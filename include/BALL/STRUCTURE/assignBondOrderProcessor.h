#ifndef BALL_STRUCTURE_ASSIGNBONDORDERPROCESSOR_H
#define BALL_STRUCTURE_ASSIGNBONDORDERPROCESSOR_H

#ifndef BALL_CONCEPT_PROCESSOR_H
	#include <BALL/CONCEPT/processor.h>
#endif

#ifndef BALL_KERNEL_ATOMCONTAINER_H
	#include <BALL/KERNEL/atomContainer.h>
#endif

#ifndef BALL_DATATYPE_HASHMAP_H
	#include <BALL/DATATYPE/hashMap.h>
#endif

#ifndef BALL_DATATYPE_HASHSET_H
	#include <BALL/DATATYPE/hashSet.h>
#endif


#ifndef BALL_KERNEL_BOND_H
	#include <BALL/KERNEL/bond.h>
#endif

#ifndef BALL_DATATYPE_OPTIONS_H
# include <BALL/DATATYPE/options.h>
#endif

#ifndef BALL_COMMON_LIMITS_H
#	include <BALL/COMMON/limits.h>
#endif

#include <queue>

#ifdef BALL_HAS_LPSOLVE
		struct _lprec;
		typedef struct _lprec lprec;
#endif

namespace BALL 
{
	/**	Bond Order assignment processor
			\ingroup StructureMiscellaneous
	*/
	
	/** Assignment of bond orders from topology information.
	 *  
	 *  Called with default options the processor computes up to 
	 *  \link Default::MAX_NUMBER_OF_SOLUTIONS Default::MAX_NUMBER_OF_SOLUTIONS\endlink 
	 *  many possible bond orders with optimal value and applies the first solution found 
	 *  to the given AtomContainer ac_.
	 *  \par
	 *  All further optimal solutions can be applied by calling
	 *  the method \link apply()  apply() \endlink. 
	 *  Additional solutions can be computed by calling the method 
	 *  \link computeNextSolution()  computeNextSolution()\endlink.
	 *  \par
	 *  <br>
	 *  Example code: <br> 
	 *  \code
 	 *  	AssignBondOrderProcessor bop;	
	 *  	bop.options.setBool(AssignBondOrderProcessor::Option::COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS, true);
	 *  	...
	 *   	sys.apply(bop);
	 *   	i = bop.getNumberOfComputedSolutions();
	 *  	bop.apply(i-1);
	 *  	... 
	 *  	while (bop.computeNextSolution())
	 *   	{
	 *    	  i++;
	 *      	bop.apply(i);
	 *  	}
	 *  	\endcode
	 */
	class BALL_EXPORT AssignBondOrderProcessor 
		: public UnaryProcessor<AtomContainer> 
	{
		protected:
			class Solution_;
			friend class Solution;
			class PQ_Entry_;
			friend class PQ_Entry_;

		public:

			// TODO: move to the GAFF-Stuff
			enum BOND_TYPES
			{
				sb,
				SB,
				db,
				DB,
				TB,
				DL,
				AB
			};

			/** @name Constant Definitions
			*/
			//@{
			/// Option names
			struct BALL_EXPORT Option
			{	
				/**	compute bond orders for all bonds of type single bond order.
				 *  @see OVERWRITE_SELECTED_BONDS 
				*/
		 		static const char* OVERWRITE_SINGLE_BOND_ORDERS;	

				/**	compute bond orders for all bonds of type double bond order.
				 *  @see OVERWRITE_SELECTED_BONDS 
				*/
				static const char* OVERWRITE_DOUBLE_BOND_ORDERS;	
				
				/**	compute bond orders for all bonds of type triple bond order.
				 *  @see OVERWRITE_SELECTED_BONDS 
				*/
				static const char* OVERWRITE_TRIPLE_BOND_ORDERS;	
				
				/**	compute bond orders for all selected bonds
				 * \par
				 *  <b>NOTE:</b> This option has higher priority than 
				 *  the OVERWRITE_BOND_ORDER-Options.
				 *  @see Option::OVERWRITE_SINGLE_BOND_ORDERS
				 *  @see Option::OVERWRITE_DOUBLE_BOND_ORDERS
				 *  @see Option::OVERWRITE_TRIPLE_BOND_ORDERS
				*/
				static const char* OVERWRITE_SELECTED_BONDS; 	
				
				/**	add hydrogens based on free valences
				*/
				static const char* ADD_HYDROGENS;

				/**	kekulize rings
				*/
				static const char* KEKULIZE_RINGS;

				/**	technique to compute all solutions
				*/
				static const char* ALGORITHM; 
				
				/** the penalty parameter file
				 */
				static const char* INIFile;	
				
				/** the maximal possible bond order
				 */
				static const char* MAX_BOND_ORDER;

				/** the maximal number of solutions to compute
				 *  If set to zero all optimal solutions will be computed.
				 *  @see Option::COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS
				 */
				static const char* MAX_NUMBER_OF_SOLUTIONS;
			
				/** compute also non-optimal solutions but not more than 
				 *  \link MAX_NUMBER_OF_SOLUTIONS MAX_NUMBER_OF_SOLUTIONS \endlink solutions.
				 *  @see Option::MAX_NUMBER_OF_SOLUTIONS
				 */
				static const char* COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS;

				/** The weighting of bond length penalties wrt valence penalties.
				 *  If set to zero, the valence penalty will not be normalized. 
				 */
				static const char* BOND_LENGTH_WEIGHTING;

			};

			/// Default values for options
			struct BALL_EXPORT Default
			{
				static const bool OVERWRITE_SINGLE_BOND_ORDERS;
				static const bool OVERWRITE_DOUBLE_BOND_ORDERS;
				static const bool OVERWRITE_TRIPLE_BOND_ORDERS;
				static const bool OVERWRITE_SELECTED_BONDS;
				static const bool ADD_HYDROGENS;
				static const bool OVERWRITE_CHARGES;
				static const bool ASSIGN_CHARGES;
				static const bool KEKULIZE_RINGS;
				static const String ALGORITHM;
				static const String INIFile;
				static const int MAX_BOND_ORDER;	
				static const int MAX_NUMBER_OF_SOLUTIONS;
				static const bool COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS;
				static const float BOND_LENGTH_WEIGHTING;
			};

			struct BALL_EXPORT Algorithm
			{
				static const String A_STAR;
				static const String ILP; 
			};
			
			//@}
		

			/** @name	Constructors and Destructors
			*/
			//@{

			BALL_CREATE(AssignBondOrderProcessor);
			
			///	Default Constructor
			AssignBondOrderProcessor();
		
			/// Copy Construcor
			AssignBondOrderProcessor(const AssignBondOrderProcessor& abop);
		
			// constructor with parameter filename //TODO
			//AssignBondOrderProcessor(const String& file_name) throw(Exception::FileNotFound);
			
			/// Destructor
			virtual ~AssignBondOrderProcessor();
			//@}

			/**	@name	Processor-related methods 
			*/
			//@{

			/// Processor method which is called before the operator()-call.
			virtual bool start();
	
			/// Clears the datastructures.
			void clear();

			/** operator () for the processor 
			 *
			 * Called with \link Default Default\endlink-options the processor computes all 
			 * possible bond orders with optimal value and 
			 * applies the first solution to the given AtomContainer ac_.
			 * \par
			 * 	<b>NOTE:</b> Having used the \link Algorithm::A_STAR Algorithm::A_STAR\endlink-option (default)
			 * 	the method \link getNumberOfComputedSolutions() getNumberOfComputedSolutions()\endlink
			 * 	will return the  number of optimal solutions+1!
			 *
			 * 	@param  ac  the AtomContainer to which the processor is applied.
			 */
			virtual Processor::Result operator () (AtomContainer& ac);

			/// Processor method which is called after the operator()-call.
			virtual bool finish();

			//@}
		
			/**	@name	Accessors
			*/
			//@{

			/** Returns the number of bonds built during the last application.
			 *
			 *  <b>NOTE:</b> bonds to newly added hydrogens are excluded.
			 *  @return  Size - the number of bonds built during the last application.
			 *  @see Option::COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS
			 *  @see Option::MAX_NUMBER_OF_SOLUTIONS
			 */
			Size getNumberOfBondOrdersSet();
		
			/** Returns the number of added hydrogens in Solution i.
			 *
			 *  @return Size - number of hydrogens added in Solution i.
			 *  @see Option::COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS
			 *  @see Option::MAX_NUMBER_OF_SOLUTIONS
			 */
			Size getNumberOfAddedHydrogens(Position i)
			{	
				if (i >= solutions_.size())
				{
					Log.error() << "AssignBondOrderProcessor: No solution with index " << i << std::endl;
					return 0;
				}
				int num_hydrogens = 0;
				
				HashMap<Atom*, int>::Iterator it = solutions_[i].number_of_virtual_hydrogens.begin(); 
				for (; it != solutions_[i].number_of_virtual_hydrogens.end(); it++)
					num_hydrogens += it->second;
				return num_hydrogens;
			}

			/** Returns the number of already computed solutions.
			 *
			 *  <b>NOTE:</b> Having applied the operator with option ALGORITHM::ASTAR
			 * 			  this method returns the number of optimal solutions+1!
			 * 	
			 * 	@return Size - number of already computed solutions. 
			 *  @see Option::COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS
			 *  @see Option::MAX_NUMBER_OF_SOLUTIONS
			 */
			Size getNumberOfComputedSolutions() {return solutions_.size();}

			/**Returns the total charge of solution i.
			
			 * @param   i index of the solution, whose charge should be computed. 
				 @return  float - total charge of solution i.  
			 */
			float getTotalCharge(Position i)
			{
				if (i >= solutions_.size())
				{
					Log.error() << "AssignBondOrderProcessor: No solution with index " << i << std::endl;

					return Limits<float>::max();
				}
				else
					return getTotalCharge(solutions_[i]);

			}
			
			/** Returns the total charge of a solution.
			 * 
			 * @param  sol solution, whose charge should be computed. 
			 * @return float -  total charge of the given solution.  
			*/
			float getTotalCharge(const Solution_& sol) 
			{
				if (sol.valid) 
				{
					return sol.total_charge;
				}
				else
				{
					return 0; 				
				} 
			}

			/** Returns the total penalty of solution i.
			 *
			 * @param    i  index of the solution, whose penalty should be returned. 
			 * @return  float -  total penalty of solution i.  
			 * @see  Option:: BOND_LENGTH_WEIGHTING;
			 */
			float getTotalPenalty(Position i)
			{
				if (i >= solutions_.size())
				{
					Log.error() << "AssignBondOrderProcessor: No solution with index " << i << std::endl;

					return Limits<float>::max();
				}
				else
					return getTotalPenalty(solutions_[i]);
			}

			/** Returns the total penalty of the given solution.
			 *
			 * @param   sol  solution, whose penalty should be returned. 
			 * @return  float -  total penalty of solution i.  
			 * @see  Option::BOND_LENGTH_WEIGHTING;
			 */
			float getTotalPenalty(const Solution_& sol) 
			{
				if (   (atom_type_normalization_factor_   < 0.00001) 
				    || (bond_length_normalization_factor_ < 0.00001)
						|| (alpha_ < 0.0001)) 
				{
					return sol.atom_type_penalty;
				}
				else
				{
					return (  (1.-alpha_) * (sol.atom_type_penalty/atom_type_normalization_factor_) 
									+ (alpha_*sol.bond_length_penalty/bond_length_normalization_factor_));
				} 
			}

			/** Applies the i-th precomputed bond order combination.
			 *
			 * Set the AtomContainer ac_'s bond orders to the ones found 
			 * in the (already computed!) i-th solution, start counting in 0!
			 * \par
			 * <b>NOTE:</b> All virtual hydrogens added to the processed AtomContainer
			 * 			 by a previous call of apply will be deleted by the current
			 * 			 call!
			 *
			 *  @param    i  index of the solution, whose bond orders should be assigned. 
			 *	@return bool - true if the i-th solution is valid.
			 */
			bool apply(Position i);

			/** Computes and applies one of the next best solutions.
			 *
			 *  Ignores the options  \link MAX_NUMBER_OF_SOLUTIONS MAX_NUMBER_OF_SOLUTIONS \endlink and
			 *											 \link COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS COMPUTE_ALSO_NON_OPTIMAL_SOLUTIONS\endlink .
			 * @return bool - false if no further solution can be found.
			 */
			bool computeNextSolution();
			//@}
			
			/** @name Assignment
			*/
			//@{
			/// assignment operator
			AssignBondOrderProcessor& operator = (const AssignBondOrderProcessor& abop);
			//@}

			/** @name Public Attributes
			*/
			//@{
			/// options
			Options options;

			/** Resets the options to default values.
			*/
			void setDefaultOptions();
			//@}
		
			/** Evaluates the AtomContainer ac's bond orders as specified in 
			 *  the Options and returns the computed penalty.
			 *
			 *  @param 	 ac	AtomContainer, whose bond orders should be evalated.
			 *  @return  float - computed penalty.
			 */
			float evaluatePenalty(AtomContainer* ac);

		protected:
			
			// Nested class storing the parameters of a solution to our ILP
			class Solution_
			{	
				friend class AssignBondOrderProcessor;

				public:
					// Default constructor
					Solution_();
				
					// Destructor
					virtual ~Solution_();
					
					// 
					void clear();
				
					// equality operator // TODO
					bool operator == (Solution_ b);

					// denotes whether the problem could be solved or not  
					bool valid;
					
					// the result : the set of bond orders for _ALL_ original bonds
					HashMap<Bond*, int> bond_orders;
					
					// the result part2: the atoms with n additional hydrogens
					HashMap<Atom*, int> number_of_virtual_hydrogens;
				
					// the virtual atoms and bonds that should be deleted when the next 
					// solution is applied
					vector<Atom*> atoms_to_delete;
					//vector<Bond*> bonds_to_delete;

					// the values of the objective function
					float atom_type_penalty;
					float bond_length_penalty;
					float total_charge;
			};
			
			// Nested class storing a priority queue entry for the A-STAR-Option
			class PQ_Entry_
			{	
				friend class AssignBondOrderProcessor;

				public:
				
					// Default constructor
					PQ_Entry_(float alpha = 0., float atom_type_normalization_factor = 1., float bond_length_normalization_factor = 1.);
								
					// Copy constructor
					PQ_Entry_(const PQ_Entry_& entry);

					// Destructor
					virtual ~PQ_Entry_();
					
					// 
					void clear();
					
					// the less operator.
					// NOTE: we want a reverse sort, hence we actually return a "greater" 
					bool operator < (const PQ_Entry_& b) const;  
					
					// the penalty
					float coarsePenalty() const {
						return ( ( (    (atom_type_normalization_factor_ < 0.0001)
										     || (bond_length_normalization_factor_ < 0.0001)
												 || (alpha_ < 0.0001)) ? 
											 estimated_atom_type_penalty :
											 ((1.-alpha_) * (estimated_atom_type_penalty / atom_type_normalization_factor_)
								    		+ (alpha_* estimated_bond_length_penalty / bond_length_normalization_factor_))));}
					
					// the bond length penalty 
					float finePenalty() const {return estimated_bond_length_penalty;}

					// the estimated atom type penalty
					float estimated_atom_type_penalty;   
					// the estimated bond length penalty
					float estimated_bond_length_penalty;

					// the bond orders 
					// the i-th entry denotes the bondorder of the i-th bond
					// unset bonds get the order 0
					vector<int> bond_orders;
			
					// the index of the bond last considered 
					Position last_bond;

					protected:
						float alpha_;
						float atom_type_normalization_factor_;
						float bond_length_normalization_factor_;
				};

			/** Reads and stores the penalty-INIFile (for example BondOrder.ini).
			 *
			 *	@return bool - false if the INIFile could not be read correctly.
			 */
			bool readAtomPenalties_()	throw(Exception::FileNotFound());
			
			/** Assigns every atom of the AtomContainer to which the
			 *  processor is applied to a block of possible valences 
			 *  and the corresponding penalties.	
			 *  
			 *  Returns false if the AtomContainer to which the processor 
			 *  is applied to has an atom with no matching penalty block. 
			 */
			bool preassignPenaltyClasses_();
		
			/**
			 * Finds the first matching SMARTS-expression in the penalty-vector
			 * and returns its index.
			 *
			 * If there is no matching expression, -1 is returned.
			 */
			int getPenaltyClass_(Atom* atom);


			/** Precomputes for every bond of the AtomContainer, to which the 
			 *	processor is applied to, the possible bond length penalties
			 *	resulting from deviation of the actual bond length to 
			 *	a standart length for bonds with same atom types and the 
			 *	chosen bond order. 
			 *	\par
			 *	If there is no information for certain atom pairs, penalty 0
			 *	is assumed.
			 *	In case of incomplete information, we assume the missing bond 
			 *	orders to be really unlikely and we set a penalty 
			 *	to 2* max_deviation_found (for this bond).
			 *
			 *	Returns false, if the AtomContainer is invalid or the processor is in an invalid state.
			 */
			bool precomputeBondLengthPenalties_();

			/** Adds missing hydrogens as virtual hydrogens to the 
			 *  given Atom and determines the possible penalty blocks. 
			 *
			 *  "virtual" means that NO  
			 *  atoms and bonds are added to the AtomContainer. 
			 *	
			 *	@param  atom the atom, to which the virtual hydrogens should be added.
			 *  @return float - the max possible penalty the atom can get, if hydrogen(s) are added. 
			 *  @see Option::ADD_HYDROGENS
			 */
			float computeVirtualHydrogens_(Atom* atom);

#ifdef BALL_HAS_LPSOLVE
			/** Setup the integer linear program.
			 */
			bool createILP_();

			/** Solve the current integer linear program and convert it into a Solution_
			 */
			bool solveILP_(Solution_& solution);
#endif
			
			/// Processor is in a useable valid state. 
			bool valid_;

			/// Processor is in an evaluation mode. Default is false
			bool evaluation_mode_;

			// Map for storing the bonds fixed orders
			// if a bond is free, the map returns 0
			map<Bond*, int> bond_fixed_;

			// all free bonds in the atom container
			vector<Bond*> free_bonds_;

			// Map for storing the bonds associated index (all bonds)
			HashMap<Bond*, Index> bond_to_index_;
			
			// Vector for mapping from variable indices onto bonds (all bonds)
			std::vector<Bond*> index_to_bond_;
	


			// ***************** datastructures for virtual hydrogen bonds ****************** 
			//
			// 	NOTE: a single virtual bond represents ALL possible hydrogen 
			// 				bonds for a given atom
			//
			// the atoms with upto n possible additional hydrogens
			HashMap<Atom*, int> number_of_virtual_hydrogens_;  
			//
			// the max number of virtual hydrogens per virtual bond index
			std::vector<int> virtual_bond_index_to_number_of_virtual_hydrogens_;  
			//	
			// the number of virtual bonds
			Position num_of_virtual_bonds_;
			//
			// the virtual bond index assigned to this atom!
			vector<Atom*> virtual_bond_index_to_atom_;
			HashMap<Atom*, int> atom_to_virtual_bond_index_;
			//
			//
			// a virtual dummy bond
			Bond* virtual_bond_;
			//
			//

			// ********************* ILP stuff ***********************
			//
			// Vector for mapping from variable indices onto free bonds in the
			// order used by the ILP
			std::vector<Bond*> ilp_index_to_free_bond_;

			// number of bond variables in the ILP
			Position ilp_number_of_free_bonds_;


			// ******************* general datastructures *********************

			// the number of bonds given (free + fixed!)
			Position total_num_of_bonds_; 

			// num of free bonds without virtual bonds!
			int num_of_free_bonds_;

			// store for all atom-indices the atoms fixed valences 
			std::vector<Position> fixed_val_;

			// storing the solutions
			vector<Solution_> solutions_;
			
			// the inverse of the atom type penalty normalization factor
			float atom_type_normalization_factor_;

			// the inverse of the bond length penalty normalization factor
			float bond_length_normalization_factor_;

			// denotes the index of the last applied solution
			// -1 if there was no valid solution applied
			int last_applied_solution_;
			
			// the AtomContainer, the processor is operating on
			AtomContainer* ac_;

			// max bond order to consider
			int max_bond_order_;

			// balance parameter between atom type and bond length penalty
			float alpha_; 

			// the max number of solutions to compute 
			int max_number_of_solutions_;

			// flag to indicate, whether also non-optimal solutions should be computed 
			bool compute_also_non_optimal_solutions_;

			// flag for adding missing hydrogens
			bool add_missing_hydrogens_;


			// //////// ************ for Algorithm::A_START ************ /////////
			/// Computes a next solution in the ASTAR - algorithm.
			bool	performAStarStep_(Solution_& sol);

			/// the priority queue 
			priority_queue<PQ_Entry_> queue_;
			
			/** Estimates the objective function f = g* + h* of the ASTAR - algorithm.
			 *  Returns true, if the entry is still valid.
			 */
			bool estimatePenalty_(PQ_Entry_& entry);
		
			/// Estimates the atom type penalty for a given unclosed atom.
			float estimateAtomTypePenalty_(Atom* atom, 
																		 Index atom_index, // the atom index
																		 int fixed_valence,  // its so far fixed valence (incl. virtual H's)
																		 int fixed_virtual_order, // its so far fixed virtual H's
																		 int num_free_bonds);   // its number of unfixed original bonds

			/// Estimates the bond length penalty for a given unclosed atom.
			//  NOTE: virtual bonds are excluded!
			float estimateBondLengthPenalty_(Index atom_index, // the atom index
																			 vector<Bond*> free_bonds, 
																			 int fixed_virtual_order,  
																			 int fixed_valence, 
																			 int num_free_bonds);

			// The penalty administration datastructures.
			//  filled by readAtomPenalties_
			//  organized in imaginarey blocks of length  
			//  block_to_length_[i], starting from 
			//  block_to_start_idx_[i] associating 
			//  block_to_start_valence_[i] to the start_idx
			vector<int> penalties_;
			vector<Position> block_to_start_idx_;
			vector<Size> block_to_length_;
			vector<int> block_to_start_valence_;
			// stores the defining element and the SMART-string of each block
			vector<pair<String, String> > block_definition_; 


			// Stores which atom belongs to which penalty block.
			// The first vector element of each atom block denotes the penalty block 
			// assigned to the atom without any additional VIRTUAL Hydrogens,
			// the second element with one additional Hydrogen and so on. 
			vector< vector<int> > atom_to_block_;
					
			// Stores the possible bond lengths penalties per order.
			HashMap<Bond*, vector<float> > bond_lengths_penalties_;

#ifdef BALL_HAS_LPSOLVE
			lprec* ilp_;
#endif
		};

} // namespace BALL 


#endif // BALL_STRUCTURE_ASSIGNBONDORDERPROCESSOR_H