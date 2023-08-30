local self, value = ...

local M = {
    { 3.2410032, -1.5373990, -0.4986159},
    {-0.9692243,  1.8759300,  0.0415542},
    { 0.0556394, -0.2040112,  1.0571490}
}

local R = (value.X * M[1][1]) + (value.Y * M[1][2]) + (value.Z * M[1][3])
local G = (value.X * M[2][1]) + (value.Y * M[2][2]) + (value.Z * M[2][3])
local B = (value.X * M[3][1]) + (value.Y * M[3][2]) + (value.Z * M[3][3])

local max = math.max(R, G, B)

R = self:call("normalize_color_component", self:call("_revert_gamma_correction", R / max ))
G = self:call("normalize_color_component", self:call("_revert_gamma_correction", G / max ))
B = self:call("normalize_color_component", self:call("_revert_gamma_correction", B / max ))

return {
    R = R,
    G = G,
    B = B
}
