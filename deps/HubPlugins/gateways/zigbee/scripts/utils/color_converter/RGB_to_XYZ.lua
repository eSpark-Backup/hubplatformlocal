local self, value = ...

local R = self:call("_apply_gamma_correction", value.R / 255)
local G = self:call("_apply_gamma_correction", value.G / 255)
local B = self:call("_apply_gamma_correction", value.B / 255)

local M = {
    {0.4123866,  0.3575915,  0.1804505},
    {0.2126368,  0.7151830,  0.0721802},
    {0.0193306,  0.1191972,  0.9503726}
}

local X = R * M[1][1] + G * M[1][2] + B * M[1][3]
local Y = R * M[2][1] + G * M[2][2] + B * M[2][3]
local Z = R * M[3][1] + G * M[3][2] + B * M[3][3]

return {
    X = X,
    Y = Y,
    Z = Z
}
