LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all; 

ENTITY reaction_meter IS
	GENERIC (
			MAX_DELAY						: INTEGER := 150_000_000;
			F_CLK								: INTEGER := 50_000_000
	);
	PORT (clock, resetn					: IN STD_LOGIC;
			resp_time						: OUT INTEGER RANGE 0 TO 2000;
			reg_delay						: IN INTEGER RANGE 0 TO 5000;
			state 							: OUT INTEGER RANGE 0 TO 12;
			irq								: BUFFER STD_LOGIC;
			buttons 							: IN STD_LOGIC_VECTOR(1 DOWNTO 0)
	);
END reaction_meter;

ARCHITECTURE behavior of reaction_meter IS

	COMPONENT prng IS
		PORT ( 
			CLK, RST, EN	: IN STD_LOGIC;
       	NUM 				: OUT STD_LOGIC_VECTOR (7 DOWNTO 0)
		);
	END COMPONENT;
	
	-- State machine
	TYPE state_t IS (IDLE, START, DELAY, COUNT, STOP);
	SIGNAL next_state : state_t := IDLE;
	
	-- Random number generation
	SIGNAL rng_en		: STD_LOGIC := '0'; 
	SIGNAL rnd_delay	: STD_LOGIC_VECTOR (7 DOWNTO 0);
	
BEGIN

	rng	: prng PORT MAP (CLK => clock, RST => '0', EN => rng_en, NUM => rnd_delay);

	PROCESS
	
		VARIABLE tick, wait_ticks  	: NATURAL RANGE 0 TO MAX_DELAY * 3 := 0;
	
	BEGIN
		WAIT UNTIL clock'event AND clock = '1';
		IF resetn = '0' THEN
			resp_time <= 0;
		ELSE
		CASE next_state IS
			WHEN IDLE =>	
				irq 		<= '0';
				-- Enable RNG and go to next next_state on button press.
				IF buttons(0) = '0' THEN
					next_state 	<= START;
					irq 			<= '1';
				END IF;

			WHEN START =>
				-- Duration until button release determines cycles of Pseudo-Random Number Generator.
				rng_en 	<= '1';
				irq 		<= '0';
				IF buttons(0) = '1' THEN
					rng_en 		<= '0';
					tick 			:= 0;
					next_state 	<= DELAY;
					irq 			<= '1';
				END IF;

			WHEN DELAY =>
				IF reg_delay /= 0 THEN
					wait_ticks 	:= reg_delay  * 50000;
				ELSE
					wait_ticks	:= (MAX_DELAY * 2) - (MAX_DELAY / 255 * TO_INTEGER(UNSIGNED(rnd_delay)));
				END IF;
				irq 			<= '0';
				tick 			:= tick + 1;
				
				--									6s				- 0 tot 3
				-- Random delay of range (MAX_DELAY * 2) - 0-MAX_DELAY
				IF tick >= wait_ticks THEN
					tick 		 	:= 0;
					
					IF buttons(1) = '1' THEN			-- Only continue if response button is not pressed.
						next_state 		<= COUNT;	-- Since tick has been set to 0; counting starts over again.
						irq 				<= '1';
					END IF;
				END IF;

			WHEN COUNT =>
				irq 		<= '0';
				tick := tick + 1;
				
				IF tick >= MAX_DELAY THEN
					-- Response timed-out; back to IDLE.
					next_state 	<= IDLE;
				ELSIF buttons(1) = '0' THEN
					-- Response button pressed.
					resp_time <= tick / (F_CLK / 1000);
					tick 		 := 0;
					next_state 	 <= STOP;
					irq 		 <= '1';
				END IF;

			WHEN STOP =>
				irq 		<= '0';
				IF buttons(1) = '1' THEN
					-- Go to IDLE when button released.
					next_state 	<= IDLE;
					irq 		 	<= '1';
				END IF;

			WHEN OTHERS =>
				tick := 0;
		END CASE; 
		state <= state_t'POS(next_state);
		
		END IF;
	END PROCESS;
END behavior;
