const message = document.getElementById("manage-message");
const arrayForm = document.getElementById("array-form");
const elementForm = document.getElementById("element-form");
const deleteForm = document.getElementById("delete-form");
const refreshButton = document.getElementById("refresh-array");
const arrayOutput = document.getElementById("array-output");

function showMessage(text, isError = false) {
    message.textContent = text;
    message.classList.toggle("error", isError);
}

function getLocation(form) {
    const group = Number(form.elements.group.value);
    const slot = Number(form.elements.slot.value);

    if (
        !Number.isInteger(group) ||
        group < 1 ||
        group > 10 ||
        !Number.isInteger(slot) ||
        slot < 1 ||
        slot > 100
    ) {
        throw new Error("Group must be 1-10 and slot must be 1-100.");
    }

    return {group, slot};
}

async function request(url, options = {}) {
    const response = await fetch(url, options);
    const data = await response.json();

    if (!response.ok) {
        throw new Error(
            data.error || `Request failed (${response.status})`
        );
    }

    return data;
}

async function displayArray() {
    try {
        const {group, slot} = getLocation(arrayForm);

        const data = await request(
            `/api/arrays/${group}/${slot}`
        );

        arrayOutput.textContent = JSON.stringify(
            data.data,
            null,
            2
        );

        showMessage(`Loaded array (${data.size} elements).`);
    } catch (error) {
        showMessage(error.message, true);
    }
}

arrayForm.addEventListener("submit", (event) => {
    event.preventDefault();
    displayArray();
});

refreshButton.addEventListener("click", displayArray);

document.getElementById("resize-form").addEventListener(
    "submit",
    async (event) => {
        event.preventDefault();

        try {
            const {group, slot} = getLocation(arrayForm);
            const size = Number(event.target.elements.size.value);

            if (
                !Number.isInteger(size) ||
                size < 1 ||
                size > 10000
            ) {
                throw new Error(
                    "Size must be between 1 and 10000."
                );
            }

            await request(`/api/arrays/${group}/${slot}`, {
                method: "PATCH",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({size})
            });

            showMessage("Array resized successfully.");
            await displayArray();
        } catch (error) {
            showMessage(error.message, true);
        }
    }
);

elementForm.addEventListener("submit", async (event) => {
    event.preventDefault();

    try {
        const {group, slot} = getLocation(arrayForm);
        const position = Number(event.target.elements.position.value);
        const value = Number(event.target.elements.value.value);

        if (
            !Number.isInteger(position) ||
            position < 1 ||
            position > 10001 ||
            !Number.isInteger(value) ||
            value < -2147483648 ||
            value > 2147483647
        ) {
            throw new Error(
                "Enter a valid position and 32-bit integer value."
            );
        }

        await request(
            `/api/arrays/${group}/${slot}/elements`,
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({position, value})
            }
        );

        showMessage("Element inserted successfully.");
        await displayArray();
    } catch (error) {
        showMessage(error.message, true);
    }
});

document.getElementById("remove-form").addEventListener(
    "submit",
    async (event) => {
        event.preventDefault();

        try {
            const {group, slot} = getLocation(arrayForm);
            const position = Number(
                event.target.elements.position.value
            );

            if (
                !Number.isInteger(position) ||
                position < 1 ||
                position > 10000
            ) {
                throw new Error(
                    "Enter a valid removal position."
                );
            }

            await request(
                `/api/arrays/${group}/${slot}/elements/${position}`,
                {
                    method: "DELETE"
                }
            );

            showMessage("Element removed successfully.");
            await displayArray();
        } catch (error) {
            showMessage(error.message, true);
        }
    }
);

deleteForm.addEventListener("submit", async (event) => {
    event.preventDefault();

    try {
        const {group, slot} = getLocation(deleteForm);

        await request(`/api/arrays/${group}/${slot}`, {
            method: "DELETE"
        });

        arrayOutput.textContent = "No array selected.";
        showMessage("Array deleted successfully.");
    } catch (error) {
        showMessage(error.message, true);
    }
});
