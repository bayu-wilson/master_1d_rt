# 1D Radiative Transfer (1D-RT) code and analysis for "Quantifying Lyman-α emissions from reionization fronts" from [(Wilson et al. 2024)](https://arxiv.org/abs/2406.14622)

The 1D-RT code was written originally by Christopher Cain and then modified, improved, and maintained by Bayu Wilson. All other analysis is done by Bayu. 


## Using the 1D-RT code

### 1) Clone the repository 
```
git clone git@github.com:bayu-wilson/1DRT_quantify_reion_fronts.git
```
### 2) Gather input data (if necessary)
If you would like density skewers from another simulation, the input data should be placed in the ```input_files/``` directory. The code is set up to read inhomogeneous density skewers used in [Davies et al. 2016](https://arxiv.org/abs/1409.0855). Contact [bwils033@ucr.edu](mailto:bwils033@ucr.edu) to request these skewers. Otherwise, uniform density skewers can be automatically generated by setting ```input_grid -> FALSE``` in ```user_inputs.h```.

### 3) Parameter space range to be be explored
The rate of incident ionizing photon production and simuluation time are scaled such that the ionization fronts will cover a portion of the Lyman alpha efficiency parameter space. This is calculated for each inhomegeneous skewer in ```parameters_input/April24_fd_input_params.py``` and for each uniform density skewer in ```variableLum_input_parameters.py```. 

```
cd parameters_input/
module load anaconda3/2020.11
python [filename]
```
The outputs are tables of simulation input parameters that are used to explore the parameter space of the Lyman-alpha efficiency in terms of I-front speed and spectral index. See ```parameters_input/readme.txt``` for more details.

### 4) Set up user-input parameters
Other user-input parameters can be edited in ```user_inputs.h```. These parameters can be updated manually or automatically using one of the ```pbs_scripts/*.pbs``` files. The parameter space studies use the parameters defined in the previous step. See ```pbs_scripts/readme.txt``` for more details.

### 5) 1D-RT outputs
1D-RT will automatically produce output files located in ```output_files/gasprops/[simulation_directory]```. Each simulation is in its own directory. For the uniform density runs, these directories are named ```ud_pp_a=[X]_L=[Y]/``` where X is the spectral index and Y is the initial luminosity. For inhomogeneous density runs, these directories are names ```fd_pp_sk[Z]_a=[X]_L=[Y]``` where X and Y are the same as before and Z is the skewer number. Within each directory, each file contains on-the-fly outputs including incident spectral index, incident flux, gas density, and emitted Lyman-alpha produced from collisional excitations with file names ```n[n_otf]gasprops.txt``` where n\_otf is the number corresponding to the otf step which is equally spaced in time. It will also save the incident spectrum (I(nu)) saved in every grid cell as the I-front passes through. This file is located in the simulation directory above but with file name ```Inu_output.txt```.

### 6) Submit batch job
Ensure the job scripts are set up correctly and then you can submit the job on a supercomputer. For example, this is what I did to submit the job on Expanse.
```
cd pbs_scripts/
sbatch [job script name].pbs
```

### 7) Processing output data
Here we read in the output files to produce a table of on-the-fly outputs (like IF speed and incident flux at the IF location). These are python scripts but they take a lot of memory and time so it is good practice to submit it with a job script. For processing the parameter space exporation simulation suite, use ```pbs_scripts/go_resultArrays.pbs```. This will run ```pp_results.py```  and save results in ```results/final_results/[processed table name].csv ```. See ```results/readme.txt``` for more details and options.  


### 8) Plotting routines and interpolation table
In ```plotting_routines/``` you can reproduce the plots in the paper (from the main text). Most importantly, ```pspace_lya_eff.py``` calculates the parameter space table and saves it as an interpolation table in ```results/interp_tables/``` as well as in a plot. See ```plotting_routines/readme.txt``` for more details on the other plotting routines. 


