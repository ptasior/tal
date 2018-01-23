import bottle
 
def methodroute(route):
    def decorator(f):
        f.route = route
        return f
    return decorator


class WebServer(object):
    def __init__(self):
        pass


    def _routeClass(self):
        for kw in dir(self):
            attr = getattr(self, kw)
            if hasattr(attr, 'route'):
                bottle.route(attr.route)(attr)


    def run(self):
        self._routeClass()
        bottle.run(host='0.0.0.0', port=8080)


    @methodroute('/')
    def index(self):
        return "ok"


