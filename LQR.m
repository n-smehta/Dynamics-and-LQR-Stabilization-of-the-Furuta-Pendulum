% === Physical Parameters ===
m = 0.02;      % Pendulum mass (kg)
l = 0.15;     % Pendulum length (m)

M = 0.1;      % Arm mass (kg)
r = 0.075;      % Arm length or (radius) (m)
J_a = (1/3)*M*(r^2);  % Arm inertia about pivot
J_p = (1/3)*m*(l^2); % pendulum inertail about pivot

g = 9.81;       % Gravity (m/s^2)

% Constant
alpha = (m*(r^2) + J_a) / J_p;
beta = (m*r*l) / J_p;
gamma = (m*g*l) / J_p;

M1 = gamma / (2 - (beta^2)/(2*alpha));
M2 = beta / (alpha*J_p*(2 - (beta^2)/(2*alpha)));
M3 = gamma / ((4*alpha/beta) - beta);
M4 = 4 / (beta*J_p*((4*alpha/beta) - beta));

A = [ 0  0 1 0;
      0  0 0 1;
      M1 0 0 0;
      M3 0 0 0];

B = [ 0;
      0;
      M2;
      M4];

Q = diag([1, 1, 100, 10]);  % Penalize pendulum angle more
R = 0.1;                    % Penalize control effort

K = lqr(A, B, Q, R);

A_cl = A - B*K;
B_cl = B;
C_cl = eye(4);
D_cl = zeros(4,1);

sys_cl = ss(A_cl, B_cl, C_cl, D_cl);

x0 = [deg2rad(90); 0; 0; 0];  % initial condition
t = 0:0.01:5;                % simulation time
[y, t, x] = initial(sys_cl, x0, t);

figure;
plot(t, x(:,1)*180/pi, 'LineWidth', 1.5); % pendulum angle in degrees
xlabel('Time (s)');
ylabel('Pendulum Angle (deg)');
title('LQR Balanced Pendulum Response');
grid on;
