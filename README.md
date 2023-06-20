# BinDayIndicator
Wemos D1 Mini and RGB Shield to show which bins to be put out.

Using a:

  WemosD1 Mini: https://www.aliexpress.com/item/32529101036.html
  
  WS2812B Shield: https://www.aliexpress.com/item/1005003449908799.html

Red, Yellow, Green Bin Indicator.

Essentially the idea is that the night before the LED starts to flash either red/green or red/yellow to indicate which bins need to go out that night. Flashing only takes place between 5p and 8p.

Set which day the bins need to go out, in this instance the bins need to go out Mon night.

 - int binNight = 1; // 1 for Monday...
  
Set the start and end times for when the LED should blink

 - int startHour = 17; // Start blinking at 5p
 - int endHour = 20;  // End blinking at 8p

Set the time zone offset for Brisbane AU

 - timeClient.setTimeOffset(10 * 60 * 60); // 10hrs ahead of GMT

Set the reference week to calculate if its a Green or Yellow week

 - int referenceGreenWeek = 23; // Green bin reference week.

