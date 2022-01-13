import numpy as np 
from scipy.stats import kurtosis
from scipy.stats import skew

def bimodality_index(b):
    n = len(b)
    return (np.power(skew(b),2) + 1)/(kurtosis(b) + (3*np.power(n-1,2))/((n-2)*(n-3)))

def balance(b):
    b = np.array(b)
    c_1 = np.sum(b > 0)
    c_2 = np.sum(b < 0)
    return np.min([c_1,c_2])/np.max([c_1,c_2])


name = "out.bin"
b = np.fromfile(name, dtype=np.double)

print("BC:", bimodality_index(b))
print("Balance:", balance(b))
