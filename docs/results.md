| Command                                                  |      Mean [µs] | Min [µs] | Max [µs] |    Relative |
|:---------------------------------------------------------|---------------:|---------:|---------:|------------:|
| `catr test_input/lorem.txt 50 100`                       |   380.6 ± 91.2 |    200.5 |   1352.5 | 1.12 ± 0.50 |
| `catr test_input/lorem.txt -r 50 100`                    |  376.6 ± 148.9 |    207.3 |   1655.4 | 1.11 ± 0.60 |
| `catr test_input/lorem.txt -rl 3 5`                      |  394.0 ± 168.7 |    139.6 |   2494.8 | 1.16 ± 0.66 |
| `catr test_input/lorem.txt -l 3 5`                       |  340.2 ± 127.1 |    169.5 |   1391.7 |        1.00 |
| `cat test_input/lorem.txt \| tail -c +50 \| head -c 101` | 1110.1 ± 177.2 |    765.1 |   1859.7 | 3.26 ± 1.33 |
| `cat test_input/lorem.txt \| head -c 100 \| tail -c +51` | 1065.8 ± 231.2 |    707.3 |   4993.0 | 3.13 ± 1.35 |
| `cat test_input/lorem.txt \| head -n 5 \| tail -n +3`    | 1104.2 ± 204.1 |    766.5 |   2444.5 | 3.25 ± 1.35 |
| `cat test_input/lorem.txt \| tail -n +3 \| head -n 5`    | 1101.8 ± 197.9 |    763.4 |   2135.9 | 3.24 ± 1.34 |
| `awk 'NR>=3 && NR<=5' test_input/lorem.txt`              |  469.0 ± 355.6 |    274.1 |  10580.3 | 1.38 ± 1.17 |
| `awk 'NR>=3 && NR<=7' test_input/lorem.txt`              |   447.0 ± 91.4 |    249.9 |   1230.5 | 1.31 ± 0.56 |
| `sed -n '3,5p' test_input/lorem.txt`                     |  396.8 ± 178.5 |    149.8 |   4908.0 | 1.17 ± 0.68 |
| `sed -n '3,7p' test_input/lorem.txt`                     |  416.0 ± 144.4 |    225.1 |   2031.8 | 1.22 ± 0.62 |
