# 1D Radiative Transfer (1D-RT) code and analysis for "Quantifying Lyman-α emissions from reionization fronts" from [(Wilson et al. 2024)](https://arxiv.org/abs/2406.14622)

The 1D-RT code was written originally by Christopher Cain and then modified, improved, and maintained by Bayu Wilson. All other analyses are done by Bayu. 

<p align="center">
    <img src="https://github.com/user-attachments/assets/b25d7c00-ccf7-43b9-8b7a-f3b658115582" alt="Description" width="600"/>
</p>
<p align="center">Figure 1. TLDR: I ran a suite of simulations (like the one above) to test and build a model that can decrease the computational cost of modeling astronomical emission from the phenomenon of interest by a factor of 10,000. More details: This is a movie showing a how an ionization-front (I-front) travels through an initially neutral, inhomogeneous density skewer from a cosmological hydrodynamic simulation at a redshift z=5.7 (around 1 billion years after big bang). A source is turned on on the left side. Then the source radiation ionizes the gas going from left-to-right and photo-heats the gas to tens of thousands of Kelvin. Together, an I-front is the perfect environment for the production of Lyman-alpha photons (high Lyman-alpha emissivity). The density fluctuations cause the I-front to move at different speeds which effectively allows us to explore the parameter space of the Lyman-alpha photon production efficiency at different I-front speeds. We used a suite of these experiments to build a model to map. For more details, read the paper!
</p>


<p align="center">
    <img src="https://github.com/user-attachments/assets/3c2b12c8-2d32-44c9-9863-5df03755eb56" alt="Description" width="400"/>
</p>
<p align="center">Figures 2. TLDR: The main result of this work is this parameter space which functions as a model to reliably and efficient map how brightly the astronomical phenomenon of interest glows. Stakeholders (astronomers) can use this model to inform their observations related to understanding cosmic history. More details. This is a visualization of our publicly available model to map Lyman-alpha emission onto larger-volume reionization simulations. Given just two parameters, spectral index and I-front speed, one can reliably and efficiently estimate the Lyman-alpha photon production efficiency. For more details, read the paper!
</p>


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

### Note on how to use the interpolation table
The following snippet of code can be used to get the estimated Lyman-alpha efficiency given a range of log10 I-front speed as well as a spectral index, $\alpha$. 
```
import numpy as np
from scipy.interpolate import griddata

interp_table = np.loadtxt("fd_parameter_space_interp_table.txt")  
bb = np.column_stack((interp_table[:,0],interp_table[:,1])) #(vIF [km/s], alpha)
Lya_efficiency = interp_table[:,2]
vIF_grid = np.logspace(np.log10(300),np.log10(5e4),1000)
logvIF_grid = np.log10(vIF_grid)

alpha = 1.5 #spectral index
estimated_Lya_efficiency = griddata(bb,Lya_efficiency,(logvIF_grid,alpha),method='linear',fill_value=0)
```

