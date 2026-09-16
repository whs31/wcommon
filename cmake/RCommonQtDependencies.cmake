include(CMakeFindDependencyMacro)

find_dependency(Qt6 REQUIRED COMPONENTS Core Gui Network)
target_link_libraries(radar::mms::common INTERFACE Qt6::Core Qt6::Gui Qt6::Network)

find_dependency(Qt6 REQUIRED COMPONENTS Positioning)
target_link_libraries(radar::mms::common::json INTERFACE Qt6::Positioning)
