

def my_func(x):
    x2 = x*x
    b = x2 + 10
    global a
    a = x2
    return b

res = my_func(2)
print(res)
print(a)
