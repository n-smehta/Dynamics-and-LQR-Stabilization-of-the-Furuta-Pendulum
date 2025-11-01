% === Physical Parameters ===
m_p = 0.01;      % Pendulum mass (kg)
l_p = 0.15;     % Pendulum length (m)
J_p = (1/3)*m_p*l_p^2;  % Pendulum inertia about pivot

m_a = 0.2;      % Arm mass (kg)
l_a = 0.1;      % Arm length (m)
J_a = (1/3)*m_a*l_a^2;  % Arm inertia about pivot

g = 9.81;       % Gravity (m/s^2)

A = [ 0      1        0        0;
      0      0     (m_p*g*l_p)/J_a   0;
      0      0        0        1;
      0      0  (g*(m_p*l_p + m_a*l_a))/J_p  0 ];

B = [ 0;
      1/J_a;
      0;
      1/J_p ];

Q = diag([1, 1, 100, 10]);  % Penalize pendulum angle more
R = 0.1;                    % Penalize control effort

K = lqr(A, B, Q, R);

A_cl = A - B*K;
B_cl = B;
C_cl = eye(4);
D_cl = zeros(4,1);

sys_cl = ss(A_cl, B_cl, C_cl, D_cl);

x0 = [0; 0; deg2rad(5); 0];  % initial condition
t = 0:0.01:5;                % simulation time
[y, t, x] = initial(sys_cl, x0, t);

figure;
plot(t, x(:,3)*180/pi, 'LineWidth', 1.5); % pendulum angle in degrees
xlabel('Time (s)');
ylabel('Pendulum Angle (deg)');
title('LQR Balanced Pendulum Response');
grid on;
