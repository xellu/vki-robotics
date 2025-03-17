from ev3dev2.motor import OUTPUT_A, OUTPUT_B
from ev3dev2.led import Leds
import time, os

from .ConfigManager import ConfigManager
from .Scheduler import ScheduleManager
from .LoggingManager import LoggingManager

Logger = LoggingManager("Core")
Config = ConfigManager(
    running = True,

    engineL = OUTPUT_A,
    engineR = OUTPUT_B
)

Logger.info("Core initialized")

Scheduler = ScheduleManager(Config)
Scheduler.run()

leds = Leds()

Scheduler.run_for(leds.set_color, 5, "LEFT", "GREEN")
Scheduler.run_for(leds.set_color, 5, "RIGHT", "AMBER")
Scheduler.run_in(leds.reset, 5)

time.sleep(10)
Config.set("running", False)

while True:
    if not Scheduler.running:
        break

    Logger.info("Waiting for Scheduler to stop")
    time.sleep(1)

Logger.info("Stopping")
os._exit(0)