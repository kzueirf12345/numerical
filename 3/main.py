import numpy as np
import matplotlib.pyplot as plt

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

plt.figure(figsize=(16, 16))
for i in range(n_paths):
    plt.plot(t, X[i], alpha=0.1, color='blue', linewidth=0.5)

plt.plot(t, np.mean(X, axis=0), color='red', linewidth=2, label='Среднее')
plt.axhline(y=mu, color='green', linestyle='--', label=f'Долгосрочное среднее (μ={mu})')
plt.title("Процесс Орнштейна-Уленбека")
plt.xlabel("Время")
plt.ylabel("X(t)")
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()