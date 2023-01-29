add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PUBLIC FILE_SET headers TYPE HEADERS FILES
		InOut.hpp
		Input.hpp
		Output.hpp
		Trigger.hpp
	PRIVATE
		InOut.cpp
		Input.cpp
		Output.cpp
		Trigger.cpp
)

if(${PLATFORM} STREQUAL "native")
	# native platform (Windows, MacOS, Linux)
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/native FILES
			native/coco/platform/InOut_cout.hpp
			native/coco/platform/Input_cout.hpp
			native/coco/platform/Output_cout.hpp
			native/coco/platform/Trigger_cout.hpp
		PRIVATE
			native/coco/platform/InOut_cout.cpp
			native/coco/platform/Input_cout.cpp
			native/coco/platform/Output_cout.cpp
			native/coco/platform/Trigger_cout.cpp
	)
elseif(${PLATFORM} MATCHES "^nrf52")
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/nrf52 FILES
			nrf52/coco/platform/InOut_GPIO.hpp
			nrf52/coco/platform/Input_GPIO.hpp
			nrf52/coco/platform/Output_GPIO.hpp
			nrf52/coco/platform/Trigger_GPIOTE_RTC0.hpp
		PRIVATE
			nrf52/coco/platform/InOut_GPIO.cpp
			nrf52/coco/platform/Input_GPIO.cpp
			nrf52/coco/platform/Output_GPIO.cpp
			nrf52/coco/platform/Trigger_GPIOTE_RTC0.cpp
	)
elseif(${PLATFORM} MATCHES "^stm32f0")
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0 FILES
			stm32f0/coco/platform/InOut_GPIO.hpp
			stm32f0/coco/platform/Input_GPIO.hpp
			stm32f0/coco/platform/Output_GPIO.hpp
			stm32f0/coco/platform/Trigger_EXTI_TIM2.hpp
		PRIVATE
			stm32f0/coco/platform/InOut_GPIO.cpp
			stm32f0/coco/platform/Input_GPIO.cpp
			stm32f0/coco/platform/Output_GPIO.cpp
			stm32f0/coco/platform/Trigger_EXTI_TIM2.cpp
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
