(async () => {
    const repo = "snoplus/rat";
    const counts = {};
    const seen = new Set();

    const currentYear = new Date().getFullYear();
    let total = 0;

    for (let year = 2010; year <= currentYear; year++) {
        let yearCount = 0;

        for (let page = 1; page <= 40; page++) {
            const query = `is:pr is:merged merged:${year}-01-01..${year}-12-31`;

            const url = `/${repo}/pulls?q=${encodeURIComponent(query)}&page=${page}`;

            const response = await fetch(url, {
                credentials: "same-origin"
            });

            if (!response.ok) {
                console.error(`HTTP ${response.status}: ${year}, page ${page}`);
                return;
            }

            const html = await response.text();
            const doc = document.implementation.createHTMLDocument("");

            doc.documentElement.innerHTML = html;

            const rows = [...doc.querySelectorAll("li")]
                .filter(li =>
                    li.querySelector(`a[href*="/${repo}/pull/"]`)
                );

            if (!rows.length) break;

            for (const li of rows) {
                const link = li.querySelector(`a[href*="/${repo}/pull/"]`);
                const prMatch = link.href.match(/\/pull\/(\d+)/);
                const authorMatch = li.textContent.match(
                    /\bby\s+(\S+)\s+was merged\b/
                );

                if (!prMatch || !authorMatch) continue;

                const prNumber = prMatch[1];
                const author = authorMatch[1];

                if (seen.has(prNumber)) continue;
                seen.add(prNumber);

                counts[author] = (counts[author] || 0) + 1;
                yearCount++;
                total++;
            }

            if (rows.length < 25) break;
        }

        if (yearCount > 0) {
            console.log(`${year}: ${yearCount} merged PRs`);
        }
    }

    const leaderboard = Object.entries(counts)
        .sort((a, b) => b[1] - a[1])
        .map(([user, mergedPRs], i) => ({
            rank: i + 1,
            user,
            mergedPRs
        }));

    console.log(`TOTAL MERGED PRs: ${total}`);
    console.log(`TOTAL CONTRIBUTORS: ${leaderboard.length}`);

    console.table(leaderboard);
})();