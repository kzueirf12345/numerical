import numpy as np
import matplotlib.pyplot as plt
import math

theta = 10
mu = 50
sigma = 1
X0 = 10
T = 1
dt = 0.0001
N = int(T / dt)
n_paths = 1000

X = np.zeros((n_paths, N))
t = np.linspace(0, T, N)
dW = np.sqrt(dt) * np.random.normal(0, 1, (n_paths, N))
X[:, 0] = X0

for i in range(1, N):
    X[:, i] = X[:, i-1] + theta * (mu - X[:, i-1]) * dt + sigma * dW[:, i-1]

plt.figure(figsize=(12, 12))
for i in range(n_paths):
    plt.plot(t, X[i], alpha=0.05, color='blue', linewidth=0.5)
    
avg = np.mean(X, axis=0);    
    
emp_var = np.var(X, axis=0, ddof=0)
emp_std = np.sqrt(emp_var)

theo_var = (sigma**2 / (2 * theta)) * (1 - np.exp(-2 * theta * t))
theo_std = np.sqrt(theo_var)

stat_theo_std = sigma**2 / (2 * theta)

plt.plot(t, avg, color='red', linewidth=2, label='Среднее')
plt.plot(t, avg - emp_std, 'r--', linewidth=1.5, label='Среднее +- СКО')
plt.plot(t, avg + emp_std, 'r--', linewidth=1.5)
plt.axhline(y=mu, color='green', linestyle='--', label=f'Долгосрочное среднее (nu={mu})')
plt.title("Процесс Орнштейна-Уленбека")
plt.xlabel("Время")
plt.ylabel("X(t)")
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()

plt.figure(figsize=(12, 12))
plt.plot(t, theo_var, label='Теоретическая дисперсия', color='red', linestyle='--')
plt.plot(t, emp_var, label='Оценка дисперсии', color='blue')
plt.axhline(y=stat_theo_std, color='green', linestyle=':', 
            label=f'Стационарная дисперсия = {stat_theo_std:.4f}')
plt.title('Дисперсия процесса Орнштейна-Уленбека')
plt.xlabel('Время'); plt.ylabel('Var[X(t)]')
plt.legend(); plt.grid(alpha=0.3)
plt.xlim(0.6, 1)
plt.show()
