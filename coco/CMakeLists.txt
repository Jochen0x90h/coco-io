add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PUBLIC FILE_SET headers TYPE HEADERS FILES
		Input.hpp
		InOutPins.hpp
		InputPins.hpp
		OutputPins.hpp
		OutputPins_nop.hpp
	PRIVATE
		Input.cpp
		InOutPins.cpp
		InputPins.cpp
		OutputPins.cpp
		OutputPins_nop.cpp
)

if(${PLATFORM} STREQUAL "native")
	# native platform (Windows, MacOS, Linux)
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/native FILES
			native/coco/platform/InOutPins_cout.hpp
			native/coco/platform/InputPins_cout.hpp
			native/coco/platform/OutputPins_cout.hpp
		PRIVATE
			native/coco/platform/InOutPins_cout.cpp
			native/coco/platform/InputPins_cout.cpp
			native/coco/platform/OutputPins_cout.cpp
	)
elseif(${PLATFORM} STREQUAL "emu")
	# emulator platform with graphical user interface (Windows, MacOS, Linux)
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/emu FILES
			emu/coco/platform/GuiRotaryButton.hpp
			emu/coco/platform/RotaryButton_emu.hpp
		PRIVATE
			emu/coco/platform/GuiRotaryButton.cpp
			emu/coco/platform/RotaryButton_emu.cpp
	)
elseif(${PLATFORM} MATCHES "^nrf52")
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/nrf52 FILES
			nrf52/coco/platform/RotaryButton_QDEC_GPIOTE.hpp
			nrf52/coco/platform/InOutPins_GPIO.hpp
			nrf52/coco/platform/InputPins_GPIO.hpp
			nrf52/coco/platform/OutputPins_GPIO.hpp
		PRIVATE
			nrf52/coco/platform/RotaryButton_QDEC_GPIOTE.cpp
			nrf52/coco/platform/InOutPins_GPIO.cpp
			nrf52/coco/platform/InputPins_GPIO.cpp
			nrf52/coco/platform/OutputPins_GPIO.cpp
	)
elseif(${PLATFORM} MATCHES "^stm32f0" OR ${PLATFORM} MATCHES "^stm32g4")
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0 FILES
			stm32f0/coco/platform/Buttons_EXTI.hpp
			stm32f0/coco/platform/InOutPins_GPIO.hpp
			stm32f0/coco/platform/InputPins_GPIO.hpp
			stm32f0/coco/platform/OutputPins_GPIO.hpp
		PRIVATE
			stm32f0/coco/platform/Buttons_EXTI.cpp
			stm32f0/coco/platform/InOutPins_GPIO.cpp
			stm32f0/coco/platform/InputPins_GPIO.cpp
			stm32f0/coco/platform/OutputPins_GPIO.cpp
	)
endif()

target_link_libraries(${PROJECT_NAME}
	coco::coco
	coco-loop::coco-loop
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		..
)

# install the library
install(TARGETS ${PROJECT_NAME}
	FILE_SET headers DESTINATION include/coco
	FILE_SET platform_headers DESTINATION include
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
