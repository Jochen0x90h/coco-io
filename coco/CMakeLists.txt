set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET headers TYPE HEADERS FILES
        DummyOutputPort.hpp
        InOutPort.hpp
        InputPort.hpp
        OutputPort.hpp
    PRIVATE
        DummyOutputPort.cpp
        InOutPort.cpp
        InputPort.cpp
        OutputPort.cpp
)

if(${PLATFORM} STREQUAL "native")
    # native platform (Windows, MacOS, Linux)
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/native FILES
            native/coco/platform/InOutPort_cout.hpp
            native/coco/platform/InputPort_cout.hpp
            native/coco/platform/OutputPort_cout.hpp
        PRIVATE
            native/coco/platform/InOutPort_cout.cpp
            native/coco/platform/InputPort_cout.cpp
            native/coco/platform/OutputPort_cout.cpp
    )
elseif(${PLATFORM} STREQUAL "emu")
    # emulator platform with graphical user interface (Windows, MacOS, Linux)
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/emu FILES
            emu/coco/platform/RotaryKnob_emu.hpp
        PRIVATE
            emu/coco/platform/RotaryKnob_emu.cpp
    )
elseif(${PLATFORM} MATCHES "^nrf52")
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gpio ${CMAKE_CURRENT_SOURCE_DIR}/nrf52 FILES
            gpio/coco/platform/InOutPort_GPIO.hpp
            gpio/coco/platform/InputPort_GPIO.hpp
            gpio/coco/platform/OutputPort_GPIO.hpp
            nrf52/coco/platform/InputDevice_GPIOTE_RTC.hpp
        PRIVATE
            gpio/coco/platform/InOutPort_GPIO.cpp
            gpio/coco/platform/InputPort_GPIO.cpp
            gpio/coco/platform/OutputPort_GPIO.cpp
            nrf52/coco/platform/InputDevice_GPIOTE_RTC.cpp
    )
elseif(${PLATFORM} MATCHES "^stm32")
    target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gpio ${CMAKE_CURRENT_SOURCE_DIR}/stm32 FILES
            gpio/coco/platform/InOutPort_GPIO.hpp
            gpio/coco/platform/InputPort_GPIO.hpp
            gpio/coco/platform/OutputPort_GPIO.hpp
            stm32/coco/platform/InputDevice_EXTI_TIM.hpp
        PRIVATE
            gpio/coco/platform/InOutPort_GPIO.cpp
            gpio/coco/platform/InputPort_GPIO.cpp
            gpio/coco/platform/OutputPort_GPIO.cpp
            stm32/coco/platform/InputDevice_EXTI_TIM.cpp
    )
endif()

target_link_libraries(${PROJECT_NAME}
    coco::coco
    coco-loop::coco-loop
    coco-device::coco-device
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ..
)

# install the library
install(TARGETS ${PROJECT_NAME}
    FILE_SET headers DESTINATION include/coco
    FILE_SET platform_headers DESTINATION include
)
