% === Physical Parameters ===
m = 0.02;      % Pendulum mass (kg)
L = 0.15;     % Pendulum length (m)

M = 0.1;      % Arm mass (kg)
r = 0.075;      % Arm length or (radius) (m)
J_arm = (1/3)*M*(r^2);  % Arm inertia about pivot

g = 9.81;       % Gravity (m/s^2)

% Constant
alpha_1 = m*(r^2) + J_arm;
alpha_2 = (2*L)/(3*(r^2)) - (m*r*L)/(2*alpha_1);
alpha_3 = (g)/(alpha_2 * (r^2));
alpha_4 = 1/(alpha_1 * alpha_2);
alpha_5 = (g/(r^2)) - (2*L*g)/(3*alpha_2*(r^4));
alpha_6 = (2*L*alpha_4) / (3*(r^2));

A = [ 0       0 1 0;
      0       0 0 1;
      alpha_3 0 0 0;
      alpha_5 0 0 0];

B = [ 0;
      0;
      -alpha_4;
      alpha_6];

Q = diag([1, 1, 100, 10]);  % Penalize pendulum angle more
R = 0.1;                    % Penalize control effort

K = lqr(A, B, Q, R);

A_cl = A - B*K;
B_cl = B;
C_cl = eye(4);
D_cl = zeros(4,1);

sys_cl = ss(A_cl, B_cl, C_cl, D_cl);

x0 = [deg2rad(5); 0; 0; 0];  % initial condition
t = 0:0.01:5;                % simulation time
[y, t, x] = initial(sys_cl, x0, t);

figure;
plot(t, x(:,3)*180/pi, 'LineWidth', 1.5); % pendulum angle in degrees
xlabel('Time (s)');
ylabel('Pendulum Angle (deg)');
title('LQR Balanced Pendulum Response');
grid on;
