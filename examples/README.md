# Examples

## stm32f4/main.cpp

Full UDS server example for STM32F4. Replace `StubCan` with `uds::stm32::Stm32Can`.

### Real hardware setup

```cpp
#include "stm32f4xx_hal.h"
extern CAN_HandleTypeDef hcan1;
uds::stm32::Stm32Can can_driver(&hcan1);
can_driver.configure_filter(0x7E0, 0x7FF);
HAL_CAN_Start(&hcan1);
HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

// ISR:
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan == &hcan1) can_driver.on_rx_irq();
}
```

### Tested configurations

| MCU       | CAN Speed | HAL      |
|-----------|-----------|----------|
| STM32F407 | 500kbps   | HAL 1.7  |
| STM32F103 | 250kbps   | HAL 1.1  |
