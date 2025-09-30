import json
import pandas as pd
import matplotlib.pyplot as plt

def load_profiler_json(file_path):
    with open(file_path, 'r') as f:
        data = json.load(f)
    return pd.DataFrame(data["traceEvents"])

def analyze_named_scope(df, function_name):
    # Filter to specified function name
    scope_df = df[df["name"] == function_name].copy()

    # Convert timestamps and durations to float
    scope_df["ts"] = scope_df["ts"].astype(float)
    scope_df["dur"] = scope_df["dur"].astype(float)

    # Convert timestamps from microseconds to milliseconds
    scope_df["start_time_ms"] = scope_df["ts"] / 1000.0
    scope_df["dur_ms"] = scope_df["dur"] / 1000.0
    scope_df["end_time_ms"] = scope_df["start_time_ms"] + scope_df["dur_ms"]

    # Calculate frame time deltas in ms
    scope_df = scope_df.sort_values("start_time_ms")
    scope_df["frame_time_delta_ms"] = scope_df["start_time_ms"].diff()

    return scope_df

def plot_frame_times(scope_df, function_name):
    plt.figure(figsize=(10, 5))
    plt.plot(scope_df["start_time_ms"], scope_df["frame_time_delta_ms"], marker="o", label=function_name)
    plt.title(f"Frame Time Deltas for '{function_name}'")
    plt.xlabel("Start Time (ms)")
    plt.ylabel("Frame-to-Frame Delta (ms)")
    plt.grid(True)
    plt.tight_layout()
    plt.legend()
    plt.show()

def print_frame_stats(scope_df):
    print("\n=== Frame Timing Stats (ms) ===")
    print(scope_df["frame_time_delta_ms"].describe())
    print("\nMax frame time spike:")
    max_spike = scope_df["frame_time_delta_ms"].max()
    print(f"{max_spike:.3f} ms")


def main():
    file_path = "terra_runtime.json"
    function_name = "Getting Current Texture"

    df = load_profiler_json(file_path)
    scope_df = analyze_named_scope(df, function_name)

    if scope_df.shape[0] < 2:
        print(f"Not enough entries found for '{function_name}' to compute frame times.")
        return

    plot_frame_times(scope_df, function_name)
    print_frame_stats(scope_df)

if __name__ == "__main__":
    main()
