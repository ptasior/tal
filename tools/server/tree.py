import threading

class Tree(object):
    def __init__(self):
        self._treeData = {'nodes':{}, 'value':''}
        self._lock = threading.Lock()


    def write(self, line):
        l = line.split('\1')
        val = l.pop()

        self._lock.acquire()
        self._write(l, val, self._treeData)
        self._lock.release()


    def list(self):
        self._lock.acquire()
        yield from self._list('', self._treeData)
        self._lock.release()


    def print(self):
        ret = ''

        self._lock.acquire()
        for l in self._print('', '', self._treeData):
            ret += l + '\n'
        self._lock.release()

        return ret


    def _print(self, prefix, name, ptr):
        if ptr['value'] != '':
            yield prefix+name+'='+ptr['value']
        else:
            yield prefix+name

        for n in ptr['nodes'].keys():
            yield from self._print(prefix+' ', n, ptr['nodes'][n])


    def _write(self, l, v, ptr):
        if l:
            h = l[0]
            if h not in ptr['nodes'].keys():
                ptr['nodes'][h] = {'nodes':{}, 'value':''}
            self._write(l[1:], v, ptr['nodes'][h])
        else: ptr['value'] = v


    def _list(self, prefix, ptr):
        if prefix: p = prefix + '\1' # Skips empty root
        else: p = ''

        if ptr['value'] != '':
            yield p+ptr['value']+'\2'

        for n in ptr['nodes'].keys():
            yield from self._list(p+n, ptr['nodes'][n])

