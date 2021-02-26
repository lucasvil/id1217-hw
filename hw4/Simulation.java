public class Simulation {
  private static final int nitroCap = 100;
  private static final int quantumCap = 40;
  private static final int slots = 3;
  private static final int maxVehicles = 8;
  private static final int maxTankers = 4;

  public static void main(String[] args) {
    if (args.length < 2) {
      System.out.println("Usage: 'java Simulation <#vehicles> <#tankers>'\nExiting...");
      System.exit(-1);
    }
    int numVehicles = (Integer.parseInt(args[0]) < maxVehicles) ? Integer.parseInt(args[0]) : maxVehicles;
    int numTankers = (Integer.parseInt(args[1]) < maxTankers) ? Integer.parseInt(args[1]) : maxTankers;
    FuelStation station = new FuelStation(nitroCap, quantumCap, slots);
    boolean type = true;

    for (int i = 0; i < numVehicles; i++) {
      Vehicle vehicle = new Vehicle(station, i, 5);
      new Thread(vehicle).start();
    }
    for (int i = 0; i < numTankers; i++) {
      FuelTank tanker = new FuelTank(station, i, type);
      new Thread(tanker).start();
      type = !type;
    }
  }
}
