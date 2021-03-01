import java.util.concurrent.atomic.AtomicInteger;

public class FuelStation {
  private final Object parking = new Object();
  private final Object fuelTank = new Object();
  private final Object refuel = new Object();
  private final int quantumCap, nitrogenCap, slotsCap;
  private AtomicInteger quantumLevel, nitrogenLevel, occupiedSlots, reservedNitrogen, reservedQuantum;

  public FuelStation(int n, int q, int s) {
    this.nitrogenCap = n;
    this.quantumCap = q;
    this.slotsCap = s;
    this.nitrogenLevel = new AtomicInteger(n);
    this.quantumLevel = new AtomicInteger(q);
    this.occupiedSlots = new AtomicInteger(0);
    this.reservedQuantum = new AtomicInteger(0);
    this.reservedNitrogen = new AtomicInteger(0);
  }

  public void refuel(String id, int nitrogenAmmount, int quantumAmmount) throws InterruptedException {
    // try to reserve some fuel
    while (!reserveFuel(nitrogenAmmount, quantumAmmount)) {
      synchronized (refuel) {
        System.out.println(id + " is waiting (refuel)");
        refuel.wait();
      }
    }

    // try to park
    while (!getParking()) {
      synchronized (parking) {
        System.out.println(id + " is waiting (parking)");
        parking.wait();
      }
    }
    System.out.printf("%s has taken a spot (%d/%d).\n", id, occupiedSlots.get(), slotsCap);

    // refuel and then leave
    getFuel(nitrogenAmmount, quantumAmmount, id);
    releaseSpot(id);
    synchronized (fuelTank) {
      fuelTank.notifyAll();
    }
  }

  public void refillNitrogenTank(String id, int nitrogenRefill) throws InterruptedException {
    System.out.printf("**%s has arrived.**\n", id);
    synchronized (fuelTank) {
      while (((nitrogenCap - nitrogenLevel.get()) < nitrogenRefill)) {
        fuelTank.wait();
      }
      System.out.printf("**%s is refueling nitrogen tank...**\n", id);
      putFuel(nitrogenRefill, 0);
      System.out.printf("**%s has refueled nitrogen: %d/%d (+%d)**\n", id, nitrogenLevel.get(), nitrogenCap,
          nitrogenRefill);
    }
    synchronized (refuel) {
      refuel.notifyAll();
    }
  }

  public void refillQuantumTank(String id, int quantumRefill) throws InterruptedException {
    System.out.printf("**%s has arrived.**\n", id);
    synchronized (fuelTank) {
      while (((quantumCap - quantumLevel.get()) < quantumRefill)) {
        fuelTank.wait();
      }
      System.out.printf("**%s is refueling quantum tank...**\n", id);
      putFuel(0, quantumRefill);
      System.out.printf("**%s has refueled quantum: %d/%d (+%d)**\n", id, quantumLevel.get(), quantumCap,
          quantumRefill);

    }
    synchronized (refuel) {
      refuel.notifyAll();
    }
  }

  private void releaseSpot(String id) {
    synchronized (parking) {
      occupiedSlots.getAndDecrement();
      parking.notify();
      System.out.printf("%s is driving away. (%d/%d)\n", id, occupiedSlots.get(), slotsCap);
    }
  }

  private synchronized boolean reserveFuel(int nitrogenAmmount, int quantumAmmount) {
    if (isFuel(nitrogenAmmount, quantumAmmount)) {
      if (nitrogenAmmount > 0)
        this.reservedNitrogen.addAndGet(nitrogenAmmount);
      if (quantumAmmount > 0)
        this.reservedQuantum.addAndGet(quantumAmmount);
      return true;
    }
    return false;
  }

  private synchronized boolean getParking() {
    if (isParking()) {
      occupiedSlots.getAndIncrement();
      return true;
    }
    return false;
  }

  private void getFuel(int nitrogenAmmount, int quantumAmmount, String id) {
    try {
      // simulate refuel time
      Thread.sleep((nitrogenAmmount + quantumAmmount) * 100);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    if (nitrogenAmmount > 0) {
      this.nitrogenLevel.addAndGet(-nitrogenAmmount);
      this.reservedNitrogen.addAndGet(-nitrogenAmmount);
    }
    if (quantumAmmount > 0) {
      this.quantumLevel.addAndGet(-quantumAmmount);
      this.reservedQuantum.addAndGet(-quantumAmmount);
    }
    System.out.printf("%s has refueled. nitrogen: %d/%d (-%d), quantum: %d/%d (-%d)\n", id, nitrogenLevel.get(),
        nitrogenCap, nitrogenAmmount, quantumLevel.get(), quantumCap, quantumAmmount);
  }

  private void putFuel(int nitrogenRefill, int quantumRefill) {
    // simulate refuel time
    try {
      Thread.sleep((nitrogenRefill + quantumRefill) * 100);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    if (nitrogenRefill > 0)
      this.nitrogenLevel.addAndGet(nitrogenRefill);
    if (quantumRefill > 0)
      this.quantumLevel.addAndGet(quantumRefill);
  }

  private boolean isFuel(int nitrogenAmmount, int quantumAmmount) {
    if ((this.nitrogenLevel.get() - this.reservedNitrogen.get()) < nitrogenAmmount)
      return false;
    if ((this.quantumLevel.get() - this.reservedQuantum.get()) < quantumAmmount)
      return false;
    return true;
  }

  private boolean isParking() {
    if ((this.slotsCap <= this.occupiedSlots.get()))
      return false;
    return true;
  }
}