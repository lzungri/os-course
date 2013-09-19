import time
import threading
from threading import Semaphore
from threading import Thread

sa = Semaphore(2)
sb = Semaphore(0)
mutex = Semaphore(1)
elems = Semaphore(8)

elements = ["A", "B", "B", "A", "B", "B", "A", "B"]

def fa():
	while True:
		sa.acquire()
		sa.acquire()
		elems.acquire()
		mutex.acquire()
		v = elements.pop(0)
		mutex.release()
		print "Soy %s, lei el valor: %s" % (threading.currentThread().name, v) 
		sb.release()
		sb.release()

def fb():
	while True:
		sb.acquire()
		elems.acquire()
		mutex.acquire()
		v = elements.pop(0)
		mutex.release()
		print "Soy %s, lei el valor: %s" % (threading.currentThread().name, v) 
		sa.release()

def fc():
	time.sleep(5)

	while True:
		mutex.acquire()
		elements.append("B")
		elements.append("A")
		elements.append("B")
		mutex.release()

		elems.release()
		elems.release()
		elems.release()


Thread(name="A", target=fa).start()
Thread(name="B", target=fb).start()
Thread(target=fc).start()

time.sleep(15)

