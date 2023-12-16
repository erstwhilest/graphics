# for i in range(5):
# 	for j in range(10):
# 		for k in range(2):
# 			print(i*20+j*2+k)

import numpy as np

v1=np.array([+0.0, +1.0, +0.0])
v2=np.array([-1.0, -0.0, -1.0])
v3=np.array([-1.0, -0.0, +1.0])

norm=np.cross(v2-v1, v3-v1)
norm/=np.linalg.norm(norm,ord=1)

print(norm)
