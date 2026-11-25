Behaviour test of menu_bus.cpp, now_playing.cpp and status_item.cpp (in the container rb; needs dbus, python3-gi).
  bash /tk/app/menu_bus/test/run.sh     builds /tmp/dbt/driver, runs scenario.py with the published library (classes called by
                                        address) and with ours, compares the logs (client.py) and what dbus-monitor saw (monitor.py)
  bash /tk/app/menu_bus/test/cov.sh     ours with gcov (after run.sh)
  bash /tk/app/menu_bus/test/asan.sh    ours with the address and undefined behaviour sanitizers (after run.sh)
Each run has a dbus-daemon of its own. The driver is controlled over the bus (test.Driver at /driver).
