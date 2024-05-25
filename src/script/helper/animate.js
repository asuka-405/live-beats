const options = {
  root: null, // Use the viewport as the root
  rootMargin: "0px", // No margin around the root
  threshold: 0.7, // Trigger when 50% of the target is visible
}

export function animate(className, toggleClass) {
  const elm = document.querySelectorAll(`.${className}`)
  const elmObserver = new IntersectionObserver((entries) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        console.log("intersecting" + entry.target.classList)
        entry.target.classList.add(toggleClass)
      } else {
        entry.target.classList.remove(toggleClass)
      }
    })
  }, options)
  elm.forEach((e) => {
    elmObserver.observe(e)
  })
}
