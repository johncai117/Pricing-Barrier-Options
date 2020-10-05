ORFLIB Release Notes
====================

VERSION 0.10.0
-------------

### Additions

1. New files `orflib/pricers/ptpricers.hpp` and `ptpricers.cpp`.  
	They contain portfolio related functions.
  
2. New files `xlorflib/xlfunctions5.cpp` and `xlorflib/xlregister5.cpp`.  
	They register and implement the following Excel callable functions:  
   ORF.PTRISK             (portfolio expected return and risk)  
   ORF.MVPWGHTS           (mvp weights)  
   ORF.MKTWGHTS           (CAPM market portfolio weights)  
   ORF.MKTRISK            (CAPM market portfolio mean return and standard deviation)  


VERSION 0.9.0
-------------

### Additions

1. New file `orflib/products/americancallput.hpp`.  
	Definition of the class AmericanCallPut.

### Modifications

1. In files `xlorflib/xlfunctions4.cpp` and `xlorflib/xlregister4.cpp`.  
	Definition and registration of the Excel function ORF.AMERBSPDE.


VERSION 0.8.0
-------------

### Additions

1. New folder `orflib/methods/pde`. It contains PDE related files.

2. New file `orflib/methods/pde/tridiagonalops1d.hpp`.  
	Definition of the class TridiagonalOperator and derived classes.

3. New file `orflib/methods/pde/pdegrid.hpp`.  
	Definition of the classes CoordinateChange and GridAxis.
	
4. New file `orflib/methods/pde/pdeparams.hpp`.  
	Definition of the class PdeParams.

5. New file `orflib/methods/pde/pderesults.hpp`.  
	Definition of the class PdeResults.

6. New file `orflib/math/interpol/interpolation1d.hpp`.  
	Definition of the class LinearInterpolation1D.

7. New files `orflib/methods/pde/pdebase.hpp` and `pdebase.cpp`.  
	Definition of the abstract base class PdeBase.
	
8. New files `orflib/methods/pde/pde1dsolver.hpp` and `pde1dsolver.cpp`.  
	Definition of the one spot dimension PDE solver.
	
9. New files `xlorflib/xlfunctions4.cpp` and `xlorflib/xlregister4.cpp`.  
	Definition and registration of the Excel function ORF.EUROBSPDE

10. In file `xlorflib/xlutils.hpp` and `xlutils.cpp`.  
	Added utility function xlOperToPdeParams.

### Modifications

1. In file `orflib/products/product.hpp`.  
   Added method Product::timeSteps. It sets up the time steps to be used in the PDE solver.  

2. In file `orflib/products/product.hpp`.  
   Added method overload Product::eval. It evaluates the product at a specific point in time.  


VERSION 0.7.0
-------------

### Additions

1. New file `orflib/math/random/primitivepolynomials.hpp`  
	It contains the encoding of the first 3,666 primitive polynomials mod 2
  
2. New files `orflib/math/random/sobolurng.hpp` and `.cpp`  
	Implementation of the Sobol sequence generator.

3. New folder `orflib/math/linalg`  
	It will contain linear algebra related routines.

4. New files `choldcmp.cpp`, `eigensym.cpp`, `spectrunc.cpp` in folder `orflib/math/linalg`  
	They contain implementations for Cholesky decomposition, eigenvalues/eigenvectors of 
	a real symmetric matrix and spectral truncation of a non-positive definite correlation matrix.

5.  Added function declarations for above algorithms in `orflib/math/linalg/linalg.hpp`

6. New Excel functions ORF.EIGENSYM() and ORF.SPECTRUNC() in `xlfunctions0.cpp`  
	They allow testing of the diagonalization and spectral truncation of a correlation matrix.

7. New file `orflib/products/asianbasketcallput.hpp`  
	It defines the payoff of an Asian call or put option on a basket of assets.
  
8. New files `orflib/pricers/multiassetbsmcpricer.hpp` and `.cpp`  
	They define the multi-asset Black-Scholes Monte Carlo pricer class.
  
9. New Excel function ORF.ASIANBASKETBSMC() implemented in the file `xlorflib/xlfunctions3.cpp`	
	Provides the Excel interface to pricing Asian baskets in Black-Scholes MC.

### Modifications

1. Added typedef for Sobol generator in file `orflib/math/random/rng.hpp`

2. Added Cholesky decomposition and correlation spectral truncation to class PathGenerator

3. Widened the ctor of EulerPathGenerator to accept an optional correlation matrix

4. Added method nAssets() to class Product in file `orflib/products/product.hpp` and to class EuropeanCallPut

5. Extended the utility function xlOperToMcParams() to recognize more values for the URNGType and PathGenType settings

6. Added linking to f2c.lib, blas.lib and lapack.lib third party libraries in the `armadillo-9.500.2/lib` folder.  
   This affects only the xlorflib project.


VERSION 0.6.0
-------------

### Additions

1. New folder `orflib/math/random`  
	It will contain files for random number generation.
  
2. New file `orflib/math/random/normalrng.hpp`  
	It defines the NormalRng class template that generates random normal deviates.
  
3. New file `orflib/math/random/rng.hpp`  
	It contains typedefs for common combinations of distributions and underlying generators.  
	This file is to be included by any other file that requires random samples.
  
4. New folder `orflib/methods/montecarlo`  
	It will contain files specific to MC path generation.
  
5. New files `orflib/methods/montecarlo/pathgenerator.hpp` and `eulerpathgenerator.hpp`  
	The first file defines the abstract base PathGenerator and the second file defines a class template 
	that generates paths using the Euler method.
  
6. New file `orflib/methods/montecarlo/mcparams.hpp`  
	It defines a struct to be used for collecting MC parameters.

7. New folder `orflib/products`  
	It will contain files specific to product payoffs.

8. New files `orflib/products/product.hpp` and `orflib/products/europeancallput.hpp`  
	The abstact product base class and the European option product class.

7. New file `orflib/math/stats/statisticscalculator.hpp`  
	It defines the base class template for classes that compute statistics of samples.
  
9. New file `orflib/math/stats/meanvarcalculator.hpp`  
	It defines the mean-variance calculator.

10. New files  `orflib/pricers/bsmcpricer.hpp` and `bsmcpricer.cpp`  
	They define the Black-Scholes Monte Carlo pricer class.
    
11. New Excel utility function xlOperToMcParams() in file `xlorflib/xlutils.hpp` and `xlutils.cpp` 

12. Two new Excel files `xlorflib/xlregister3.cpp` and `xlfunctions3.cpp`  
	They define the Excel function ORF.EUROBSMC

13. Added all solutions to homeworks 2-5.


VERSION 0.5.0
-------------

### Additions

1. New utility functions toContCmpd() and fromContCpd() in file `orflib/utils.hpp`     
   They convert an interest rate to/from  continuous compounding from/to compounding with a given annual frequency.

2. New folder `orflib/math/optim`  
  It will contain root finding and optimization routines.
	
3. New file `orflib/math/optim/roots.hpp`  
  It contains the root finding routines zbrak and rtsec from NR C++ 3ed.

4. New Excel callable functions ORF.TOCC, ORF.FROMCC, ORF.POLYBRACKET and ORF.POLYSECANT.


VERSION 0.4.0
-------------

### Additions

1. New file `orflib/sptrmap.hpp`
	It defines the SPtrMap class template.

2. New folder `orflib/market` with two new files `yieldcurve.hpp` and `yieldcurve.cpp`.  
	They define and implement the YieldCurve class.

3. Two new files `orflib/market/market.hpp` and `market.cpp`  
	They define and implement the Market singleton class.

4. New files `orflib/util.hpp` for generic utility fiunctions.   
	It contains a function for trimming strings.
	
5. Two new utility functions xlAddTick() and xlStripTick() in files `xlorflib/xlutils.hpp` and `.cpp`.  
	These are used for adding and removing the tick-tock mark after the name of the object handle.

6. Two new files `xlorflib/xlfunctions2.cpp` and `xlorflib/xlregister2.cpp`  
	They implement and register the following Excel functions  
	ORF.MKTLIST  
	ORF.MKTCLEAR  
	ORF.YCCREATE  
	ORF.DF  
	ORF.FWDDF  
	ORF.SPOTRATE  
	ORF.FWDRATE  


VERSION 0.3.0
-------------

### Additions

1. New file `orflib/math/matrix.hpp`.   
  It defines aliases for the Vector and Matrix classes to be used in numerical work.

2. New folder `orflib/math/interpol` with two new files `piecewisepolynomial.hpp` and `piecewisepolynomial.cpp`.  
	They implement the class PiecewisePolynomial.

3. Two new files `xlorflib/xlutils.hpp` and `xlorflib/xlutils.cpp`.  
	They implement the utility functions xlOperToVector(), xlVectorToOper(), xlOperToMatrix() and xlMatrixToOper().
	
4. The following Excel callable functions have been added:  
	ORF.MATRIXECHO  
	ORF.PPOLYEVAL  
	ORF.PPOLYINTEGRAL  
	ORF.PPOLYSUM  
	These are used for testing the Matrix and PiecewisePolynomial classes.

### Modifications

1. Orflib now has dependency on the armadillo C++ matrix library. 
  The "C/C++->Additional Include Directories" project settings have been updated.


VERSION 0.2.0
-------------

### Additions

1. New folder `orflib/math/stats` to contain statistics related source files.

2. New files `orflib/math/stats/errorfunction.hpp` and `errorfunction.cpp`.  
	Definition of class ErrorFunction.

3. New files `orflib/math/stats/univariatedistribution.hpp` and `normaldistribution.hpp`.  
	Definition of `UnivariateDistribution` and `NormalDistribution` classes.

4. New folder `orflib/math/pricers` to contain pricing related source files.

5. New files `orflib/math/pricers/simplepricers.hpp` and `simplepricers.cpp`.  
	Definition of the functions fwdPrice(), digitalOptionBS() and europeanOptionBS().

6. New file `xlorflib/xlfunctions1.cpp`.  
	Added definition of the following functions:  
	xlOrfErf(), xlOrfInvErf(), xlOrfNormalCdf(), xlOrfNormalInvCdf(), xlOrfFwdPrice(), xlOrfDigiBS() and xlOrfEuroBS().

7. In file `xlorflib/xlregister1.cpp`  
	Added registration of Excel callable functions:   
	ORF.ERF, ORF.INVERF, ORF.NORMALCDF, ORF.NORMALINVCDF, ORF.FWDPRICE, ORF.DIGIBS, ORF.EUROBS.

8. New spreadsheet example `examples/Excel/orflib-0.2.0-examples.xlsx`  
	

### Modifications

1. Removed unnecessary file `orflib/empty.cpp`


VERSION 0.1.0
-------------

### Additions

1. New folder `orflib` with start-up code for the core library.

2. `orflib/orflib.vcxproj` VC++ project file, that builds the core library `orlib.lib`.

3. New folder `xlorflib` with start-up code for the Excel interface.

4. `xlorflib/xlorflib.vcxproj` VC++ project file, that builds the `xlorflib.xll` Excel add-in.

5. `orflib.sln` Visual Studio solution file, that contains the projects `orflib` and `xlorflib`.

6. This release notes file.

7. `.gitignore` file for managing which files to keep under revision control.
