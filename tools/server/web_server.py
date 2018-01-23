import bottle
import app_globals
 
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
        bottle.run(host='0.0.0.0', port=8080, debug=True)


    @methodroute('/')
    def index(self):
        return bottle.template('index')


    @methodroute('/network')
    def network(self):
        # TODO Mutex here
        hide = bottle.request.query.get('hide')
        if not hide or hide == '':
            hide = []
        else:
            hide = hide.split(',')
        return bottle.template('network',
                    glob=app_globals.logger.globalLog,
                    cli=app_globals.logger.clientsLog,
                    hide=hide
                )


    @methodroute('/tree')
    def tree(self):
        txt = app_globals.tree.print()
        txt = txt.replace('\n', '<br>')
        txt = txt.replace(' ', '<span class="treeSpan">|</span>')
        txt = txt.replace('=', ' = ')
        return bottle.template('tree', tree=txt)


    @methodroute('/maintenance')
    def maintenance(self):
        return bottle.template('base', base="Not implemented yet")


