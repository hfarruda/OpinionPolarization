echo "Example using the arguments file (with rewiring)."
./simulation network.adjlist out 1.473 -1 1 15000000 0.1 COS_X COS_X_2 1 1
python3 measurements.py