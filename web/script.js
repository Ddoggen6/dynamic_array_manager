const form = document.getElementById("create-array-form");
const message = document.getElementById("create-message");

function showMessage(text, isError = false) {
    message.textContent = text;
    message.classList.toggle("error", isError);
}

async function refreshDashboard() {
    try {
        const response = await fetch("/api/groups");
        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.error || "Could not load groups.");
        }

        renderGroups(data.groups);
    } catch (error) {
        showMessage(error.message, true);
    }
}

function renderGroups(groups) {
    const container = document.getElementById("groups-container");
    const occupiedElement = document.getElementById("occupied-slots");
    const availableElement = document.getElementById("available-slots");

    container.replaceChildren();

    let occupiedTotal = 0;
    let availableTotal = 0;

    groups.forEach((group) => {
        occupiedTotal += group.occupied;
        availableTotal += group.available;

        const card = document.createElement("article");
        card.className = "group-card";

        const heading = document.createElement("h3");
        heading.textContent = `Group ${group.group}`;

        const occupied = document.createElement("p");
        occupied.textContent = `Occupied slots: ${group.occupied}`;

        const available = document.createElement("p");
        available.textContent = `Available slots: ${group.available}`;

        const slots = document.createElement("div");
        slots.className = "slot-grid";

        group.arrays.forEach((array) => {
            const slot = document.createElement("div");
            slot.className = "slot occupied";
            slot.title = `Slot ${array.slot}: ${array.size} elements`;
            slots.appendChild(slot);
        });

        for (
            let i = group.arrays.length;
            i < group.occupied + group.available;
            i++
        ) {
            const slot = document.createElement("div");
            slot.className = "slot empty";
            slots.appendChild(slot);
        }

        card.append(heading, occupied, available, slots);
        container.appendChild(card);
    });

    document.getElementById("total-groups").textContent = groups.length;
    occupiedElement.textContent = occupiedTotal;
    availableElement.textContent = availableTotal;
}

form.addEventListener("submit", async (event) => {
    event.preventDefault();

    const size = Number(
        document.getElementById("array-size").value
    );

    if (!Number.isInteger(size) || size < 1 || size > 10000) {
        showMessage(
            "Array size must be between 1 and 10000.",
            true
        );
        return;
    }

    try {
        const response = await fetch("/api/arrays", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({size})
        });

        const data = await response.json();

        if (!response.ok) {
            throw new Error(
                data.error || "Array creation failed."
            );
        }

        showMessage(
            `Array created in Group ${data.group}, Slot ${data.slot}.`
        );

        form.reset();
        await refreshDashboard();
    } catch (error) {
        showMessage(error.message, true);
    }
});

refreshDashboard();
