import time
import threading

class ScheduleManager:
    def __init__(self, config):
        self.config = config
        self.running = False

        self.queue = [
            #{
            #   "func": callable
            #   "args": list,
            #   "kwargs": dict,
            #   "run_at": timestamp
            #}
        ]
    
    def run(self):
        threading.Thread(target=self.loop).start()

    def loop(self):
        self.running = True
        while True:
            if not self.config.get("running"): break

            for task in self.queue:
                if time.time() >= task["run_at"]:
                    task["func"](*task["args"], **task["kwargs"])

        self.running = False

    def run_in(self, func, seconds, *args, **kwargs):
        """
        schedules a task to run in specified amout of seconds
        """

        self.queue.append({
            "func": func,
            "args": args,
            "kwargs": kwargs,
            "run_at": time.time() + seconds
        })

    def run_for(self, func, seconds, *args, **kwargs):
        """
        runs a task for a specified amount of time (needed for some ev3dev funcs)
        """

        def run_for_func():
            func(*args, **kwargs)
            time.sleep(seconds)

        threading.Thread(target=run_for_func).start()