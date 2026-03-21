import json, glob, sys


def analyze_p_values(p_vals):
    if not p_vals:
        return False, "no data"
    n = len(p_vals)
    bad = [p for p in p_vals if p < 0.01 or p > 0.99]
    mean = sum(p_vals) / n
    low_decile = sum(1 for p in p_vals if p < 0.1) / n
    high_decile = sum(1 for p in p_vals if p > 0.9) / n
    
    issues = []
    if bad and len(bad) / n > 0.05:
        issues.append(f"{len(bad)} suspicious (<0.01 or >0.99)")
    if low_decile > 0.15:
        issues.append(f"low_decile={low_decile:.2%}")
    if high_decile > 0.15:
        issues.append(f"high_decile={high_decile:.2%}")
    
    status = "PASS" if not issues else "WARN"
    summary = "; ".join(issues) if issues else "OK"
    return status == "PASS", f"{status}: {summary} | min={min(p_vals):.4f}, max={max(p_vals):.4f}, mean={mean:.4f}"


print("=== Verification Report ===\n")
all_ok = True

for fname in glob.glob("tests_chi2*.json"):
    with open(fname) as f:
        data = json.load(f)
    pvals = [t["final_p_value"] for t in data.get("tests", [])]
    ok, msg = analyze_p_values(pvals)
    if not ok: 
        all_ok = False
    print(f"[{'NORM' if ok else 'NENORM'}] {fname}\n    {msg}\n")

for fname in sorted(glob.glob("tests_rng_lag_*.json")):
    with open(fname) as f:
        data = json.load(f)
    lag = "unknown"
    try:
        lag = fname.split("_lag_")[1].split(".")[0]
    except: 
        pass
    pvals = [t["final_p_value"] for t in data.get("tests", [])]
    ok, msg = analyze_p_values(pvals)
    if not ok: 
        all_ok = False
    print(f"[{'NORM' if ok else 'NENORM'}] {fname} (lag={lag})\n    {msg}\n")

print("=== Summary ===")
print("ALL TESTS PASSED" if all_ok else "SOME TESTS NEED REVIEW")
sys.exit(0 if all_ok else 1)