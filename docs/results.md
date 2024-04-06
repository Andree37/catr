| Command | Mean [µs] | Min [µs] | Max [µs] | Relative |
|:---|---:|---:|---:|---:|
| `catr test_input/chungus.txt 50 100` | 558.1 ± 344.2 | 55.9 | 3544.8 | 1.51 ± 1.45 |
| `catr test_input/chungus.txt -r 50 100` | 369.2 ± 270.5 | 44.0 | 3668.6 | 1.00 |
| `catr test_input/chungus.txt -rl 3 5` | 787.8 ± 332.2 | 43.9 | 1595.9 | 2.13 ± 1.80 |
| `catr test_input/chungus.txt -l 3 5` | 808.3 ± 299.3 | 75.0 | 1862.5 | 2.19 ± 1.80 |
| `cat test_input/chungus.txt \| tail -c +50 \| head -c 101` | 2076.5 ± 289.0 | 1615.4 | 3046.4 | 5.62 ± 4.19 |
| `cat test_input/chungus.txt \| head -c 100 \| tail -c +51` | 1298.4 ± 358.1 | 662.4 | 3314.9 | 3.52 ± 2.75 |
| `cat test_input/chungus.txt \| head -n 5 \| tail -n +3` | 1105.9 ± 308.4 | 648.5 | 2462.1 | 3.00 ± 2.35 |
| `cat test_input/chungus.txt \| tail -n +3 \| head -n 5` | 2309.5 ± 440.4 | 1693.4 | 9306.1 | 6.26 ± 4.74 |
| `awk 'NR>=3 && NR<=5' test_input/chungus.txt` | 1785647.6 ± 16590.3 | 1761689.1 | 1810032.1 | 4836.56 ± 3543.54 |
| `awk 'NR>=3 && NR<=7' test_input/chungus.txt` | 1786416.4 ± 10023.9 | 1770655.0 | 1798999.9 | 4838.64 ± 3544.88 |
| `sed -n '3,5p' test_input/chungus.txt` | 105938.1 ± 1660.7 | 102544.2 | 108701.3 | 286.94 ± 210.26 |
| `sed -n '3,7p' test_input/chungus.txt` | 106536.3 ± 1459.6 | 103745.5 | 108905.7 | 288.56 ± 211.44 |
