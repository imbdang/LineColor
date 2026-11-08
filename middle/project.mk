MIDDLE_DIRS := middle/m_clock \
			   middle/m_time \
			   middle/m_gpio \
			   middle/m_exti \
			   middle/m_pwm \
			   middle/m_i2c \
			   middle/m_sensor

COMPONENTS_SRCDIRS		+= $(MIDDLE_DIRS)
COMPONENTS_ADD_INCLUDES	+= $(MIDDLE_DIRS)
