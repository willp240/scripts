filepath = "rat.pplxint10.206725.log"
test1 = "banana"
test2 = "orange"
with open(filepath) as fp:
   line = fp.readline()
   cnt = 1
   while line:
      if test1 in line:
         print(line.strip())
      if test2 in line:
         print(line.strip())
      line = fp.readline()
      cnt += 1
