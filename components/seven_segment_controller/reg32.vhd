LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY reg32 IS
	PORT ( clock, resetn, address : IN STD_LOGIC;
		D : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		byteenable : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
		Q : OUT STD_LOGIC_VECTOR(47 DOWNTO 0)
	);
END reg32;

ARCHITECTURE Behavior of reg32 IS

	SIGNAL to_HEX : STD_LOGIC_VECTOR(47 DOWNTO 0);
	
	COMPONENT hex7seg IS
		PORT ( hex : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
			display : OUT STD_LOGIC_VECTOR(0 TO 6)
		);
	END COMPONENT hex7seg;
	
BEGIN
	
	h0: hex7seg PORT MAP (D(3 DOWNTO 0), to_HEX(6 DOWNTO 0));
	h1: hex7seg PORT MAP (D(7 DOWNTO 4), to_HEX(13 DOWNTO 7));
	h2: hex7seg PORT MAP (D(11 DOWNTO 8), to_HEX(20 DOWNTO 14));
	h3: hex7seg PORT MAP (D(15 DOWNTO 12), to_HEX(27 DOWNTO 21));
	
	h4: hex7seg PORT MAP (D(3 DOWNTO 0), to_HEX(37 DOWNTO 31));
	h5: hex7seg PORT MAP (D(7 DOWNTO 4), to_HEX(44 DOWNTO 38));

	PROCESS
	BEGIN
		WAIT UNTIL clock'event AND clock = '1';
		IF resetn = '0' THEN
			Q <= (others => '0');
		ELSE
			IF address = '1' THEN
				IF byteenable(0) = '1' THEN
					-- Bits for HEX0 and HEX1 are written.
					Q(13 DOWNTO 0) <= to_HEX(13 DOWNTO 0); END IF;
				IF byteenable(1) = '1' THEN
					-- Bits for HEX2 and HEX3 are written.
					Q(27 DOWNTO 14) <= to_HEX(27 DOWNTO 14); END IF;
			ELSE
				IF byteenable(0) = '1' THEN
					-- Bits for HEX4 and HEX5 are written.
					Q(44 DOWNTO 31) <= to_HEX(44 DOWNTO 31); END IF;
			END IF;
		END IF;
	END PROCESS;
END Behavior;
