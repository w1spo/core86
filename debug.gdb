break *0x7c00
break *0x7c30 
break *0x7c50  

continue

x/5i $eip

# Wykonaj far jump
si

# Sprawdź czy jesteśmy w 32-bit mode
set architecture i386
x/5i $eip