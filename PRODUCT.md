# CloudStore Plugin - Product Documentation

## Product Overview

CloudStore is a secure, cloud-based storage service for RDK (Reference Design Kit) devices that enables applications to persistently store and retrieve key-value data across device lifecycles and account contexts. Built on the Thunder/WPEFramework platform, CloudStore provides a robust, scalable storage solution with enterprise-grade security and multi-device synchronization capabilities.

## Key Features

### 1. Secure Cloud Storage
- **End-to-End Encryption**: TLS/SSL encrypted communication with cloud infrastructure
- **Bearer Token Authentication**: Integration with RDK SecurityAgent for secure access
- **Identity-Based Access Control**: Per-partner, per-account, and per-device data isolation

### 2. Flexible Data Scoping
- **Account-Level Storage**: Data synchronized across all devices associated with an account
- **Device-Level Storage**: Device-specific data isolated from other devices
- **Namespace Isolation**: Application-specific data containers for multi-tenancy

### 3. Persistent Key-Value Storage
- **Simple API**: Store, retrieve, and delete key-value pairs
- **Time-To-Live (TTL) Support**: Automatic data expiration for temporary storage
- **Namespace Management**: Bulk operations for namespace-level data cleanup

### 4. Real-Time Notifications
- **Value Change Events**: Automatic notifications when data is modified
- **Multi-Client Support**: Multiple applications can subscribe to storage events
- **Asynchronous Delivery**: Non-blocking notification mechanism

## Use Cases

### Application Settings Persistence
Store application preferences, user settings, and configuration data that persists across device reboots and software updates. Data can be scoped to account level for consistent user experience across devices.

**Example Scenario:**
- Video streaming app stores playback quality preferences
- Settings synchronized across user's STB, TV, and mobile devices
- Preferences persist after factory reset when user signs back in

### Multi-Device State Synchronization
Enable applications to maintain consistent state across multiple devices belonging to the same account. Perfect for "continue watching" features and cross-device user experiences.

**Example Scenario:**
- User watches movie on living room STB
- Playback position stored in account-scoped storage
- User can resume from exact position on bedroom TV
- Watchlist and favorites automatically synchronized

### Temporary Session Data
Store ephemeral data with automatic expiration using TTL functionality. Ideal for session tokens, temporary cache, and time-sensitive information.

**Example Scenario:**
- Store authentication tokens with 24-hour expiration
- Cache API responses with configurable TTL
- Temporary user preferences during guest sessions

### Device-Specific Configuration
Maintain device-specific settings and calibration data that should not be shared across devices.

**Example Scenario:**
- Audio/video calibration settings per device
- Network configuration specific to device location
- Device-specific parental controls and restrictions

### Application Data Backup
Provide automatic cloud backup for critical application data, protecting against device failures and enabling seamless device replacements.

**Example Scenario:**
- Game progress and achievements stored in cloud
- Email and messaging app data backup
- Smart home configuration and automation rules

## API Capabilities

### JSON-RPC Interface

The CloudStore plugin exposes a JSON-RPC 2.0 API accessible via Thunder framework:

#### setValue
```javascript
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "CloudStore.1.setValue",
  "params": {
    "scope": "account",
    "namespace": "com.example.app",
    "key": "userPreference",
    "value": "darkMode",
    "ttl": 86400  // 24 hours (optional)
  }
}
```

#### getValue
```javascript
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "CloudStore.1.getValue",
  "params": {
    "scope": "account",
    "namespace": "com.example.app",
    "key": "userPreference"
  }
}
```

#### deleteKey
```javascript
{
  "jsonrpc": "2.0",
  "id": 3,
  "method": "CloudStore.1.deleteKey",
  "params": {
    "scope": "account",
    "namespace": "com.example.app",
    "key": "userPreference"
  }
}
```

#### deleteNamespace
```javascript
{
  "jsonrpc": "2.0",
  "id": 4,
  "method": "CloudStore.1.deleteNamespace",
  "params": {
    "scope": "account",
    "namespace": "com.example.app"
  }
}
```

### Event Notifications

Applications can subscribe to value change notifications:

```javascript
{
  "jsonrpc": "2.0",
  "method": "CloudStore.1.valueChanged",
  "params": {
    "scope": "account",
    "namespace": "com.example.app",
    "key": "userPreference",
    "value": "lightMode"
  }
}
```

### C++ Interface

Native C++ applications can use the IStore2 interface:

```cpp
Exchange::IStore2* store = /* obtain interface */;
string value;
uint32_t ttl;
uint32_t result = store->GetValue(
    Exchange::IStore2::ScopeType::ACCOUNT,
    "com.example.app",
    "userPreference",
    value,
    ttl
);
```

## Integration Benefits

### For Application Developers

1. **Simplified Data Management**
   - No need to implement custom storage backends
   - Automatic cloud synchronization
   - Built-in security and encryption

2. **Cross-Platform Consistency**
   - Same API across all RDK devices
   - Consistent behavior across device types
   - Standardized error handling

3. **Reduced Development Time**
   - Ready-to-use storage solution
   - No infrastructure management required
   - Well-documented API with examples

### For System Integrators

1. **Centralized Storage Management**
   - Single point of control for device storage
   - Consistent security policies
   - Unified monitoring and analytics

2. **Scalable Infrastructure**
   - Cloud-based backend handles millions of devices
   - Automatic scaling based on demand
   - Geographic distribution for low latency

3. **Simplified Deployment**
   - Thunder plugin architecture for easy integration
   - Standard RDK deployment mechanisms
   - Configurable via RFC or static configuration

## Performance Characteristics

### Latency
- **Typical Response Time**: 50-200ms depending on network conditions
- **Timeout Configuration**: Configurable per-request timeout (default: reasonable value for network operations)
- **Idle Connection Management**: Automatic connection keep-alive and reconnection

### Throughput
- **Concurrent Operations**: Non-blocking asynchronous notification delivery
- **Connection Pooling**: Single persistent gRPC connection per plugin instance
- **Efficient Serialization**: Protocol Buffer binary format for minimal overhead

### Storage Limits
- **Key Size**: Limited by gRPC message size (typically configurable)
- **Value Size**: Suitable for configuration data and small datasets
- **Namespace Isolation**: No cross-namespace query overhead

## Reliability & Availability

### Fault Tolerance
- **Automatic Retry Logic**: Built-in gRPC retry mechanisms
- **Connection Failure Detection**: Immediate remote process crash detection
- **Graceful Degradation**: Error codes for service unavailability

### Data Durability
- **Cloud Backend Persistence**: Data stored in distributed cloud infrastructure
- **Multi-Region Replication**: Backend handles data redundancy
- **Consistent Error Reporting**: Clear error codes for all failure scenarios

### High Availability
- **Stateless Client Design**: Plugin can be restarted without data loss
- **Service Discovery**: RFC-based dynamic endpoint configuration
- **Connection Management**: Automatic reconnection on network recovery

## Security & Privacy

### Data Protection
- **Transport Security**: TLS 1.2+ for all cloud communication
- **Authentication**: Bearer token validation for every request
- **Authorization**: Identity-based access control (Partner/Account/Device)

### Privacy Compliance
- **Data Isolation**: Strict namespace and scope-based separation
- **No Cross-Account Access**: Account data accessible only to account owner
- **TTL-Based Deletion**: Automatic data expiration support

### Audit & Compliance
- **Request Logging**: All operations logged for audit trails
- **Error Tracking**: Comprehensive error reporting for debugging
- **Version Control**: API versioning for backward compatibility

## Platform Requirements

### Software Requirements
- Thunder/WPEFramework R4.4.1 or later
- gRPC++ library
- Protocol Buffers library
- Linux-based RDK platform

### Optional Dependencies
- IARMBus (for system manager integration)
- RFC API (for dynamic configuration)
- SecurityAgent plugin (for authentication)

### Network Requirements
- Outbound HTTPS/gRPC connectivity
- Configurable endpoint (hostname:port)
- Support for TLS 1.2+ protocols

## Configuration & Deployment

### Plugin Configuration
CloudStore can be configured via Thunder configuration file or RFC:

**Thunder Config:**
```json
{
  "locator": "libWPEFrameworkCloudStore.so",
  "classname": "CloudStore",
  "startmode": "Activated",
  "configuration": {
    "uri": "cloudstore.example.com:443"
  }
}
```

**RFC Override:**
- Parameter: `Device.X_RDK_STORE2_URI`
- Format: `hostname:port`
- Precedence: RFC value overrides config file

### Deployment Modes
- **In-Process**: Plugin runs in Thunder process (default)
- **Out-of-Process**: Plugin runs in separate process for isolation
- **Container**: Plugin runs in containerized environment

## Support & Documentation

### API Version
- Current Version: 1.0.1
- Semantic Versioning: Major.Minor.Patch
- Backward Compatibility: Maintained within major version

### Developer Resources
- API Reference: Available via Thunder framework documentation
- Code Examples: Included in test directories (l0test, l2test)
- Integration Guide: See ARCHITECTURE.md for technical details

### Error Handling
Comprehensive error codes for all operations:
- `ERROR_NONE`: Success
- `ERROR_INVALID_INPUT_LENGTH`: Invalid parameters
- `ERROR_UNKNOWN_KEY`: Key not found
- `ERROR_PENDING_CONDITIONS`: Time sync required
- `ERROR_GENERAL`: Service unavailable or network error

## Future Enhancements

Planned features and improvements:
- Bulk get/set operations for improved efficiency
- Query and search capabilities within namespaces
- Data migration and export utilities
- Enhanced monitoring and metrics
- Support for structured data types beyond key-value pairs
