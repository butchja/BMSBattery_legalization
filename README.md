# BMSBattery_legalization

// ****************************************************************************************
// *                                                                                      *
// * Legalization Firmware for STM32F103C8 Arduino Nano Style Boards                      *
// * for BMSBattery S-LCD3 and SN12SNN 72V Sine Motor Driver*                             *
// * * might be compatible to others                                                      *
// *                                                                                      *
// * Allows limiting to maximum legal speeds of 25km/h                                    *
// * Reduces the thumb throttle to 6km/h (traction aid only)                              *
// * Output power is limited to device's minimum I/2                                      *
// * A logical condition needs to be met to unlock legal limitations                      *
// * that might be used for an off-road mode                                              *
// *                                                                                      *
// * TODO:                                                                                *
// * - adding RFID support RC-522 SPI based                                               *
// * - reducing power output furthermore or limiting displayed power                      *
// *                                                                                      *
// * Creator                                                                              *
// *  butchja@arcor.de                                                                    *
// *                                                                                      *
// ****************************************************************************************
