clear all;
clc;

N = 1000;
maxV = 20.8;
maxI = 3.65;

Id = @(Vd,Voc,Vt,Isc)Isc * (1 - exp((Vd-Voc)/Vt));

X = linspace(0, maxV, N);
Vt = linspace(0.5, 3, N);

maxP = zeros(N);

for n = linspace(1,N,N);
    Y = Id(X, maxV, Vt(n), maxI);
    maxP(n) = max(X.*Y);
end

%plot(Vt, maxP);
plot(X, Id(X,maxV,1.178,maxI));
hold off;
