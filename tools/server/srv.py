#!/usr/bin/env python3

# No security whatsoever.
# You are playing with friends, if they cheat, the only reasonable solution is
#  to change friends. It will solve a bunch of other problems as well and
#  will make you a happier person in long run.

from pprint import pprint
import threading

import app_globals

app_globals.init()

threading.Thread(target=app_globals.server.run).start()
app_globals.web.run()

