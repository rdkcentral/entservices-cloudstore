# CloudStore Plugin

A Thunder/WPEFramework plugin providing secure cloud-based key-value storage for RDK devices.

## Overview

CloudStore enables applications to store and retrieve persistent data in the cloud with account and device-level scoping. It provides a secure, scalable storage solution with multi-device synchronization capabilities.

## Features

- Secure cloud storage via gRPC
- Account and device-level data scoping
- Time-to-live (TTL) support for automatic expiration
- Real-time value change notifications
- JSON-RPC API for easy integration
- Bearer token authentication

## Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical architecture and component details
- [PRODUCT.md](PRODUCT.md) - Product features, use cases, and API documentation
- [CHANGELOG.md](plugin/CHANGELOG.md) - Version history and changes

## Building

```bash
# Install dependencies
./build_dependencies.sh

# Build the plugin
./cov_build.sh
```

## Plugin Structure

```
plugin/                    # CloudStore plugin implementation
├── CloudStore.cpp        # Main plugin class
├── CloudStore.h
├── CloudStoreImplementation.cpp
├── CloudStoreImplementation.h
├── grpc/                 # gRPC client implementation
│   ├── Store2.h
│   └── secure_storage/   # Protocol buffer definitions
├── l0test/              # Unit tests
└── CMakeLists.txt

Tests/                    # Test infrastructure
├── L1Tests/
└── L2Tests/
```

## Configuration

Configure the plugin via Thunder configuration:

```json
{
  "cloudstore": {
    "uri": "cloudstore.example.com:443",
    "mode": "Off",
    "startuporder": "50"
  }
}
```

## API

JSON-RPC methods:
- `CloudStore.1.setValue` - Store key-value pair
- `CloudStore.1.getValue` - Retrieve value by key
- `CloudStore.1.deleteKey` - Delete specific key
- `CloudStore.1.deleteNamespace` - Delete all keys in namespace

Events:
- `CloudStore.1.valueChanged` - Notification when value changes

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE) file for details.

## Contributing

This repository follows RDK contribution guidelines. Please see CODEOWNERS for maintainer information.
