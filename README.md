# OpinionPolarization
Software for simulating a dynamic of opinion formation and transmission in social networks.
More information regarding the dynamic is described in:

[Henrique Ferraz de Arruda, Felipe Maciel Cardoso, Guilherme Ferraz de Arruda, Alexis R. Hernández, Luciano da Fontoura Costa, Yamir Moreno,
Modelling how social network algorithms can influence opinion polarization, Information Sciences, Volume 588, 2022.](https://doi.org/10.1016/j.ins.2021.12.069)
[(arXiv)](https://arxiv.org/abs/2102.00099)


## Compile

For compiling this code, both Clang and GCC can be used.
```
 clang -Ofast simulation.cpp -o simulation
```

```
 gcc -Ofast simulation.cpp -o simulation
```

## Documentation
```
./simulation network_name output_name phi min_value max_value n_block_iterations delta transmission_type reception_type n_blocks rewire_dynamics
```

* `network_name` - Filename of the input network, and the format of this file must be adjacency list. It is necessary to include the same connection in both directions. See an example as follows:

```
0 1 2
1 0
2 0 5
3 4 5
4 3 5
5 2 3 4
```

* `output_name` - Output filenames;
* `phi` - Parameter phi that controls the distribution probability function;
* `min_value` - Minimun opinion value; 
* `max_value` - Maximun opinion value.
* `n_block_iterations` - Number of iteretions;
* `delta` - Parameter delta, which is a float number;
* `transmission_type` - One of the following options must be chosen:
  - `COS_X`: P<sub>t</sub> <sup>pol</sup>(x);
  - `COS_X_CUT`: P<sub>t</sub> <sup>sim</sup>(x);
  - `EQUAL_TRANSMISSION`: P<sub>t</sub> <sup>uni</sup>(x);
  - `MIXED_TRANSMISSION`: P<sub>t</sub> <sup>all</sup>(x);
* `reception_type` - One of the following options must be chosen: 
  - `COS_X`: P<sub>d</sub> <sup>I</sup>;
  - `COS_X_2`: P<sub>d</sub> <sup>II</sup>;
  - `EQUAL`: P<sub>d</sub> <sup>III</sup>.
* `n_blocks` - Number of blocks of iterations. Each block saves a separated file with `n_block_iterations` iterations.
* `rewire_dynamics` - 0 and 1 represent the dynamic with and without rewiring, respectively. 


## Examples of use

There are two possibilities for executing the software. As the first option, all the parameters must be listed as arguments.

```
./simulation network.adjlist out 1.473 -1 1 15000000 15000000 0.1 COS_X COS_X_2 1 1
```

The second possibility is to create a configuration file, named "parameters.txt". Here, it shall be executed without including parameters.  

```
./simulation
```

For both cases, the parameters must follow the order shown in the Documentation section. 

## Acknowledgements
Henrique F. de Arruda acknowledges FAPESP for sponsorship (grants 2018/10489-0 and 2019/16223-5). Guilherme F. de Arruda and Yamir Moreno acknowledge support from Intesa Sanpaolo Innovation Center. Luciano da F. Costa thanks CNPq (Grant No. 307085/2018-0) and NAP-PRP-USP for sponsorship. Yamir Moreno acknowledges partial support from the Government of Aragón and FEDER funds, Spain through grant ER36-20R to FENOL, and by MINECO and FEDER funds (grant FIS2017-87519-P). Research carried out using the computational resources of the Center for Mathematical Sciences Applied to Industry (CeMEAI) funded by FAPESP (grant 2013/07375-0). This work has been supported also by FAPESP grants 2015/22308-2. The founders had no role in study design, data collection, and analysis, decision to publish, or preparation of the manuscript.

## License
This software is under the following license.

```
Copyright (c) 2022 OpinionPolarization

OpinionPolarization (c) by HenriqueFerraz de Arruda, FelipeMaciel Cardoso, 
Guilherme Ferraz de Arruda, Alexis R. Hernández, Luciano da Fontoura Costa, 
and Yamir Moreno

OpinionPolarization is licensed under a
Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

You should have received a copy of the license along with this
work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>. 

Software provided as is and with absolutely no warranty, express or implied, 
with no liability for claim or damage.
```
