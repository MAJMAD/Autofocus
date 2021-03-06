# Autofocus Semicon West 2022
The Autofocus Semicon West 2022 demo system highlights the sqeed and precision supported by both piezo and voice coil mechanics. It showcases an extended range of motion uncommon for piezo systems with the E-709.1C1L/P-725.8CDE2 controller/mechanic pair and an even large range of motion with the C-414.13030/V-308.753030 controller/mechanic, a multi-purpose single axis system designed for hyper compatibility with applications calling for it's use as an X, Y, or Z axis system. It expands upon the previous autofocus demo system, including the V-308 as an additional second Z-axis and utilizes an X-axis C-413.2GA/V-528.1AA controller/mechanic pair rather than the prior C-891/V-408 pair, allowing for additional space to include the C-414 and V-308 in the system and further expansion of the system in the future to include a Y-axis mechanic controlled by the same C-413.2GA unit.

As implemented, the Autofocus Semicon West 2022 demo system offers an algorithmic approach to automating the determination of an optimal Z position for viewing any planar fields with a precision to 1% of the total travel range of each Z-axis stage, 8um and 70um respectively. This level of precision is not limited to these levels by the system mechanics nor the system algorithm, and these factors of the system can be leveraged to provide levels of precision that fully utilize the system mechanic's sub nanometer resolution. The system limitation is its ability to handle the computational intensity of its algorithm, which is managed by a PIPython program running on a Raspberry Pi 4B, and was chosen as a system host device for it's readily deployable characteristics rather than it's computational power.

# CAD Models
The CAD Models repository features the CAD models attributable to the majority of the Autofocus Semicon West 2022 demo system's individual components.

# Development
The Development repository features the development files used for and produced by the Autofocus Semicon West 2022 demo system's development process.

# Documentation
The Documentation repository features the user manuals attributable to the majority of the Autofocus Semicon West 2022 demo system's individual components as well as documentation of the system overall.

# Source Code
The Source Code repository features the source code used by the Raspberry Pi host device that facilitates the rest of the system's main operations.

# Acknowledgements
Acknowledgements and special thanks to Billy Machado, Dave Rego, Stefan Vorndran, and Michael Oldenburg for their guidance and work which supported the development and realization of this demo system.
