# CloudStore Plugin - Architecture Documentation

## Overview

The CloudStore plugin is a Thunder/WPEFramework-based service that provides secure cloud-based key-value storage capabilities for RDK devices. It enables applications to store, retrieve, and manage data with account and device-level scoping through a secure gRPC connection to a distributed gateway secure storage service.

## System Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     RDK Device                               │
│  ┌────────────────────────────────────────────────────────┐ │
│  │           Thunder/WPEFramework                         │ │
│  │  ┌──────────────┐                                      │ │
│  │  │  CloudStore  │◄──JSON-RPC API                      │ │
│  │  │   Plugin     │                                      │ │
│  │  └──────┬───────┘                                      │ │
│  │         │ IStore2 Interface                            │ │
│  │  ┌──────▼────────────────────┐                         │ │
│  │  │  CloudStoreImplementation │                         │ │
│  │  └──────┬────────────────────┘                         │ │
│  │         │                                               │ │
│  │  ┌──────▼─────────┐                                    │ │
│  │  │  gRPC Store2   │                                    │ │
│  │  │  Client Stub   │                                    │ │
│  │  └──────┬─────────┘                                    │ │
│  └─────────┼──────────────────────────────────────────────┘ │
│            │ gRPC/TLS                                        │
└────────────┼─────────────────────────────────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│  Distributed Gateway            │
│  Secure Storage Service         │
│  (Cloud Infrastructure)         │
└─────────────────────────────────┘
```

## Component Architecture

### 1. CloudStore Plugin (Thunder Plugin Interface)

**File:** `plugin/CloudStore.cpp`, `plugin/CloudStore.h`

The main plugin class that integrates with the Thunder framework:
- Implements `PluginHost::IPlugin` for lifecycle management
- Implements `PluginHost::JSONRPC` for JSON-RPC API exposure
- Manages plugin initialization, deinitialization, and configuration
- Handles remote connection notifications
- Provides security token acquisition from SecurityAgent

**Key Responsibilities:**
- Plugin lifecycle management (Initialize/Deinitialize)
- Configuration parsing (URI endpoint, RFC parameters)
- JSON-RPC method registration (setValue, getValue, deleteKey, deleteNamespace)
- Event notification propagation (valueChanged events)
- Remote process connection monitoring

### 2. CloudStoreImplementation

**File:** `plugin/CloudStoreImplementation.cpp`, `plugin/CloudStoreImplementation.h`

Implements the core storage interface:
- Implements `Exchange::IStore2` interface for storage operations
- Implements `Exchange::IConfiguration` for plugin configuration
- Delegates storage operations to the gRPC Store2 client

**Interface Methods:**
- `SetValue()` - Store key-value pairs with optional TTL
- `GetValue()` - Retrieve values by key
- `DeleteKey()` - Remove specific keys
- `DeleteNamespace()` - Remove all keys in a namespace
- `Register()/Unregister()` - Notification subscription management

### 3. gRPC Store2 Client

**File:** `plugin/grpc/Store2.h`

Handles secure cloud storage communication:
- Establishes gRPC channel with SSL/TLS or insecure credentials
- Implements IStore2 interface with remote storage backend
- Manages authentication via bearer tokens
- Handles device/account/partner identification
- Provides notification callbacks for value changes

**Key Features:**
- Secure gRPC communication with configurable endpoints
- Bearer token authentication integration
- Client-side timeout management (GRPC_TIMEOUT)
- Connection idle timeout configuration
- Multi-client notification support
- Time synchronization awareness for TTL handling

### 4. Protocol Buffer Definitions

**File:** `plugin/grpc/secure_storage/secure_storage.proto`

Defines the gRPC service contract:
- SecureStorageService with CRUD operations
- Message types for requests/responses
- Scope definitions (ACCOUNT, DEVICE)
- TTL and expiration time handling

## Data Flow

### SetValue Operation Flow

1. Application calls JSON-RPC `setValue` method
2. CloudStore plugin validates and routes to IStore2 interface
3. CloudStoreImplementation delegates to gRPC Store2 client
4. gRPC Store2 prepares request with:
   - Partner ID (from /opt/www/whitebox/partnerId.txt)
   - Account ID (from /opt/www/authService/accountId.txt)
   - Device ID (from /opt/www/authService/deviceId.txt)
   - Authorization bearer token
   - Namespace, key, value, and TTL
5. Request sent to cloud gateway over gRPC/TLS
6. Response received and converted to Thunder error codes
7. Notification dispatched to registered listeners

### GetValue Operation Flow

1. Application requests value via JSON-RPC `getValue`
2. Request routed through CloudStore to gRPC client
3. gRPC client sends GetValueRequest with authentication
4. Response processed with TTL calculation:
   - If TTL present: use directly
   - If expire_time present: calculate remaining time (requires time sync)
5. Value returned to application

## Plugin Framework Integration

### Thunder Integration Points

- **PluginHost::IShell** - Plugin service interface for configuration and system services
- **JSON-RPC** - Automatic JSON-RPC API generation from IStore2 interface
- **Remote Process Communication** - Out-of-process plugin support
- **Worker Pool** - Asynchronous notification dispatch
- **Service Registration** - Plugin versioning and metadata

### Configuration

Plugin configuration via Thunder config:
```json
{
  "uri": "hostname:port",
  "mode": "Off|Local|Container",
  "startuporder": "number"
}
```

Configuration can be overridden via RFC (Radio Frequency Control):
- RFC parameter: `Device.X_RDK_STORE2_URI`
- Environment variable: `STORE2_URI`

## Security Architecture

### Authentication & Authorization

1. **Bearer Token Authentication**
   - Obtained from SecurityAgent plugin via IAuthenticate interface
   - Included in gRPC metadata as "authorization: Bearer <token>"
   - Token acquisition during plugin initialization

2. **TLS/SSL Transport**
   - gRPC channel with SSL credentials for remote endpoints
   - Insecure credentials for localhost/development endpoints

3. **Device Identity**
   - Partner ID, Account ID, Device ID from secure file locations
   - Identity included in every storage request

### Data Scoping

- **ACCOUNT Scope** - Data shared across devices for same account
- **DEVICE Scope** - Data isolated to specific device
- Namespace isolation for application separation

## Dependencies

### External Dependencies

- **WPEFramework/Thunder** - Plugin framework (R4.4.1+)
- **gRPC++** - Remote procedure call framework
- **Protobuf** - Serialization library
- **IARMBus** (optional) - For system manager integration (time sync)
- **RFC API** (optional) - For runtime configuration

### Internal Interfaces

- `Exchange::IStore2` - Storage interface definition
- `Exchange::IConfiguration` - Configuration interface
- `Exchange::IAuthService` - Authentication service interface

## Testing Architecture

### L0 Tests (Unit Tests)
Located in `plugin/l0test/` and `plugin/grpc/l0test/`
- Mock-based unit testing
- Component isolation testing
- CloudStore plugin interface verification

### L2 Tests (Integration Tests)
Located in `plugin/grpc/l2test/`
- gRPC stub testing
- End-to-end integration testing
- Multi-component interaction verification

## Performance Considerations

- **Asynchronous Notifications** - Using Thunder worker pool to prevent blocking
- **Connection Idle Timeout** - Configurable gRPC channel idle timeout
- **Request Timeout** - Per-request deadline management
- **Connection Pooling** - Single persistent gRPC connection per plugin instance

## Error Handling

Thunder error codes mapped from gRPC status:
- `INVALID_ARGUMENT` → `ERROR_INVALID_INPUT_LENGTH`
- `NOT_FOUND` → `ERROR_UNKNOWN_KEY`
- Time sync required → `ERROR_PENDING_CONDITIONS`
- Other errors → `ERROR_GENERAL`

## Scalability & Reliability

- Multi-client notification support with thread-safe client list
- Automatic remote process crash detection and plugin deactivation
- Configurable retry and timeout policies
- Stateless client design for horizontal scalability
