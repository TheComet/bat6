# -*- coding: utf-8 -*-
"""
Created on Thu Oct  8 10:24:36 2015
@author: thecomet
"""

R_S     = 50e-3  # current sense resistor for 0A-5A range
I_O     = 5      # maximum output current
V_IN    = 36     # input voltage
f_S     = 500e3  # switching frequency
R_DS_ON = 30e-3

# minimum inductor value
L_min = (V_IN**2/4.0) / (0.3*f_S*I_O*V_IN)

# power of current sense resistor
P_RS = R_S**2 * I_O

# peak current in inductor
I_PEAK = I_O + (V_IN**2/4.0) / (2.0*f_S*L_min*V_IN)

print("inductor selection")
print("    inductance (min): {}H".format(L_min))
print("    peak current: {}A".format(I_PEAK))

print("current sense resistor selection")
print("    resistance: {} Ohm".format(R_S))
print("    power: {}W".format(P_RS))

print("MOSFET selection")
print("    ")
