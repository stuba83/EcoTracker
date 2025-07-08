# EcoTracker - Environmental Monitoring System

## Project Overview

EcoTracker is an environmental monitoring system designed to measure air quality and noise levels in Cartago, Costa Rica. This project combines IoT hardware with cloud services to provide real-time environmental data.

## General Objective

Create a basic environmental monitoring system that captures, processes, and visualizes air quality and noise level data using IoT technology and Azure Cloud services.

## Hardware Components

### Main Devices
- **Wio Terminal** - Main microcontroller with LCD display
- **MXChip IoT Kit** - Complementary IoT device
- **GPS Module** - For measurement geolocation
- **Air Quality Sensor** - Measurement of particles and gases
- **Integrated Microphone** - For noise level measurement

### Development Tools
- Arduino IDE
- Azure Subscription (IoT Hub + Fabric Real Time Intelligence)

## Implementation Phases

### Phase 1: Wio Terminal Familiarization (1-2 weeks)
**Objectives:**
- Set up Arduino IDE development environment
- Understand Wio Terminal architecture
- Perform initial tests with integrated components

**Activities:**
- Installation and configuration of libraries
- Basic LCD screen programming
- Testing buttons and integrated sensors
- Creating basic user interface

### Phase 2: Sensor Integration (2-3 weeks)
**Objectives:**
- Connect and calibrate external sensors
- Implement environmental data reading
- Develop local visualization system

**Activities:**
- Physical connection of air quality sensor
- GPS module integration
- Microphone data reading programming
- LCD dashboard development
- Basic alert system implementation

### Phase 3: Connectivity and Cloud (2-3 weeks)
**Objectives:**
- Establish WiFi connectivity
- Configure Azure services
- Implement real-time data transmission

**Activities:**
- WiFi connection configuration on Wio Terminal
- Azure IoT Hub creation and configuration
- Data transmission protocol development
- Fabric Real Time Intelligence configuration
- Basic web dashboard implementation

## Project Deliverables

### Hardware
- Functional EcoTracker device with local measurements
- Real-time visualization system
- Hardware technical documentation

### Software
- Documented source code for Wio Terminal
- Azure services configuration
- Web dashboard for remote visualization

### Documentation
- User manual
- Installation and configuration guide
- Learning process documentation
- Base code for future expansion

## Testing Location

**Initial site:** Cartago, Costa Rica
- Urban measurements at different city points
- Comparative air quality analysis
- Urban noise pattern monitoring

## Technologies Used

### Hardware
- Seeed Studio Wio Terminal
- Specialized environmental sensors
- GPS and WiFi communication modules

### Software and Services
- Arduino IDE
- Microsoft Azure IoT Hub
- Azure Fabric Real Time Intelligence
- MQTT/HTTP protocols for IoT

## Getting Started

### Prerequisites
- Arduino IDE installed
- Azure subscription active
- Basic knowledge of C++ programming
- Git installed for version control

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/EcoTracker.git
   cd EcoTracker
   ```

2. **Set up Arduino IDE:**
   - Install Wio Terminal board package
   - Install required libraries

3. **Hardware Setup:**
   - Connect sensors according to wiring diagrams
   - Power up the Wio Terminal

4. **Azure Configuration:**
   - Create IoT Hub instance
   - Configure device credentials

## Project Structure

```
EcoTracker/
├── README.md
├── docs/
│   ├── hardware-setup.md
│   ├── phase-guides/
│   └── api-documentation.md
├── src/
│   ├── wio-terminal/
│   ├── mxchip/
│   └── azure-config/
├── hardware/
│   ├── schematics/
│   └── datasheets/
├── tests/
└── assets/
    └── images/
```

## Contributing

This is a learning laboratory project. Feel free to:
- Report issues
- Suggest improvements
- Submit pull requests
- Share your own environmental monitoring experiences

## License

This project is open source and available under the [MIT License](LICENSE).

## Next Steps

1. **Immediate:** Development environment setup
2. **Short term:** Initial Wio Terminal testing
3. **Medium term:** Complete sensor integration
4. **Long term:** Azure deployment and data analysis

## Project Notes

- This is a laboratory project for educational purposes
- Development will be incremental to facilitate learning
- Documentation will be updated as the project progresses
- Code will be modular to allow future expansions

---

**Project Started:** July 2025  
**Developer:** Steven Uba  
**Version:** 1.0
