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


    def print(self, br='\n'):
        ret = ''

        self._lock.acquire()
        for l in self._list():
            ret += l + br
        self._lock.release()

        return ret


    def _write(self, l, v, ptr):
        # TODOAssert if mutex locked
        if l:
            h = l[0]
            if h not in ptr['nodes'].keys():
                ptr['nodes'][h] = {'nodes':{}, 'value':''}
            self._write(l[1:], v, ptr['nodes'][h])
        else: ptr['value'] = v


    def _list(self, prefix, ptr):
        # TODO Assert if mutex locked
        if prefix: p = prefix + '\1' # Skips empty root
        else: p = ''

        if ptr['value'] != '':
            yield p+ptr['value']+'\2'

        for n in ptr['nodes'].keys():
            yield from self._list(p+n, ptr['nodes'][n])

