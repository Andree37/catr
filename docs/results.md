| Command | Mean [µs] | Min [µs] | Max [µs] | Relative |
|:---|---:|---:|---:|---:|
| `catr test_input/largest_lorem.txt 50 100` | 304.7 ± 94.2 | 121.6 | 1293.0 | 1.06 ± 0.40 |
| `catr test_input/largest_lorem.txt -r 50 100` | 311.3 ± 69.2 | 128.8 | 879.4 | 1.09 ± 0.33 |
| `catr test_input/largest_lorem.txt -rl 3 5` | 318.6 ± 70.0 | 154.3 | 688.3 | 1.11 ± 0.34 |
| `catr test_input/largest_lorem.txt -l 3 5` | 286.6 ± 60.5 | 139.0 | 740.2 | 1.00 |
| `cat test_input/largest_lorem.txt \| tail -c +50 \| head -c 101` | 1928.9 ± 141.9 | 1634.3 | 2702.9 | 6.73 ± 1.50 |
| `cat test_input/largest_lorem.txt \| head -c 100 \| tail -c +51` | 1018.4 ± 120.4 | 767.0 | 1551.4 | 3.55 ± 0.86 |
| `cat test_input/largest_lorem.txt \| head -n 5 \| tail -n +3` | 1024.6 ± 124.7 | 797.1 | 1518.8 | 3.57 ± 0.87 |
| `cat test_input/largest_lorem.txt \| tail -n +3 \| head -n 5` | 1908.2 ± 109.3 | 1676.2 | 2322.5 | 6.66 ± 1.46 |
| `awk 'NR>=3 && NR<=5' test_input/largest_lorem.txt` | 2263.2 ± 74.6 | 2108.7 | 2541.3 | 7.90 ± 1.69 |
| `awk 'NR>=3 && NR<=7' test_input/largest_lorem.txt` | 2270.5 ± 99.4 | 2068.1 | 2833.3 | 7.92 ± 1.71 |
| `sed -n '3,5p' test_input/largest_lorem.txt` | 368.5 ± 68.8 | 218.4 | 699.8 | 1.29 ± 0.36 |
| `sed -n '3,7p' test_input/largest_lorem.txt` | 364.5 ± 63.3 | 201.7 | 872.9 | 1.27 ± 0.35 |
